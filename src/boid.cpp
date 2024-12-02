#include "boid.h"
using namespace BoidInfo;

void Boid::process(BoidContainer*& bc, const unsigned* indices, int neighbours, std::vector<vec3> homes) {
    dir = normalize(velocity);
    move(bc, indices, neighbours, homes);
    limitSpeed();
    update();
}

void Boid::move(BoidContainer*& bc, const unsigned* indices, int neighbours, std::vector<vec3> homes) {
    int numNeighbors = neighbours;
    int numFamily = 0;  // only move by family rules if near family. otherwise, boid will move towards origin due to subtraction in alignment and cohesion checks

    // alignment variable
    vec3 avgVel(0);

    // cohesion
    vec3 avgCentre(0);

    // separation variables
    vec3 avgMove(0);
    // if being chased, ignore all prey. define separate attacking and fleeing so that any behaviour defined
    // before fully seen (i.e., deciding to chase before finding a predator in boid list) can be undone
    vec3 avgMoveAtt(0);
    vec3 avgMoveFlee(0);

    bool isBeingChased = false;    // am i being chased?
    bool isInPursuit = false;      // am i chasing prey (intercepting or chasing)
    bool isInChasing = false;      // am i chasing prey (chasing only)
    vec3 closestPrey = vec3(1e9);  // chase closest prey instead of group if it's within chase distance
    float closestPreyDist = 1e9;
    float biggestFearWeight = 0;

    for (int i = 0; i < neighbours; ++i) {
        Boid* otherBoid = bc->boids[indices[i]];
        if (otherBoid->ID == ID) continue;
        float distFromBoid = distance(pos, otherBoid->pos); // todo: only need sq distance for comparisons
        // numNeighbors++;
        if (isFamily(type, otherBoid->type)) {
            // stay within group of same boid type
            numFamily++;

            // alignment
            avgVel += otherBoid->velocity;

            // cohesion
            avgCentre += otherBoid->pos;

            // separation
            if (distFromBoid < getBoidSepDistance(type)) {
                avgMove += pos - otherBoid->pos;
            }
        } else if (!SM::canBoidsAttack) {
            // to avoid altering if-statement structure in case i decide to remove this
            // todo: add slight flocking behaviour to non-enemmies
        } else {
            if (isPreyTo(type, otherBoid->type)) {
                if (distFromBoid <= getBoidInterceptDistance(type)) {
                    isBeingChased = true;
                }
                biggestFearWeight = max(biggestFearWeight, getBoidFearWeight(type, otherBoid->type));
                avgMoveFlee += (pos - otherBoid->pos) * biggestFearWeight;
            } else if (isPredatorTo(type, otherBoid->type)) {
                isInPursuit = isInPursuit || distFromBoid <= getBoidInterceptDistance(type);
                if (distFromBoid <= getBoidChaseDistance(type)) {
                    // move towards goal
                    isInChasing = true;
                    if (distFromBoid < closestPreyDist) {
                        closestPrey = otherBoid->pos;
                        closestPreyDist = distFromBoid;
                    }
                    avgMoveAtt -= (pos - otherBoid->pos) * getBoidGoalWeight(type);
                } else if (distFromBoid <= getBoidInterceptDistance(type)) {
                    // intercept goal
                    // doing it like this means predators are drawn towards larger groups more than single prey
                    avgMoveAtt -= (pos - (otherBoid->pos + otherBoid->velocity)) * getBoidGoalWeight(type);
                }
            }
        }
    }

    // Determine where the home is
    if (getHomeValidation(type)) {
        if (Util::sqDist(pos, currentHome) > homeRange * homeRange) {
            hasHome = false;
            float consideredHomeDist = isBeingChased ? newHomeDistFlee * newHomeDistFlee : newHomeDistDrift * newHomeDistDrift;
            for (vec3 home : homes) {
                if (Util::sqDist(pos, home) < consideredHomeDist) {
                    // printf("%.2f\n", Util::sqDist(pos, currentHome));
                    // Util::printVec3(home);
                    currentHome = home;
                    hasHome = true;
                    break;
                }
            }
        }
    }

    if (numNeighbors != 0) {
        if (numFamily != 0) {
            // alignment
            avgVel /= numFamily;
            velocity += (avgVel - velocity) * getBoidMatchingFactor(type);

            // cohesion
            avgCentre /= numFamily;
            velocity += (avgCentre - pos) * getBoidCenteringFactor(type);
        }

        // separation
        if (isBeingChased) {
            // printf("a(%.2f)", biggestFearWeight);
            // if (hasHome) {
            //     avgMoveFlee -= (pos - currentHome) * biggestFearWeight * .5f;  // fleeing home
            // }
            // Util::printVec3(avgMoveFlee);
            velocity += avgMoveFlee * getBoidAvoidFactor(type) * 100.f;
        } else if (isInPursuit) {
            if (isInChasing) {
                // chase closest target only
                avgMoveAtt = -(pos - closestPrey) * getBoidGoalWeight(type);
            }
            velocity += avgMoveAtt * getBoidAvoidFactor(type);
        }
        if (hasHome) {
            // velocity -= (pos - currentHome) * 0.005f; // drift towards home
        }
        velocity += avgMove * getBoidAvoidFactor(type);
    }
}

void Boid::limitSpeed() {
    float tspeed = dot(velocity, velocity);
    // can use glsl's isnan in a compute shader, so this is fine: https://registry.khronos.org/OpenGL-Refpages/gl4/html/isnan.xhtml
    if (glm::any(glm::isnan(velocity))) resetVelocity();
    if (tspeed < getBoidMinSpeed(type) * getBoidMinSpeed(type)) {
        velocity *= 1.1f;
        dir = normalize(velocity);
    } else {
        float max_speed = getBoidMaxSpeed(type);
        if (tspeed > max_speed * max_speed) {
            velocity = normalize(velocity) * max_speed;
        }
    }
}

void Boid::update() {
    float tf = 1;
    if (pos.x < WORLD_BOUND_LOW)
        velocity.x += tf;
    if (pos.x > WORLD_BOUND_HIGH)
        velocity.x -= tf;
    if (pos.y < WORLD_BOUND_LOW)
        velocity.y += tf;
    if (pos.y > WORLD_BOUND_HIGH)
        velocity.y -= tf;
    if (pos.z < WORLD_BOUND_LOW)
        velocity.z += tf;
    if (pos.z > WORLD_BOUND_HIGH)
        velocity.z -= tf;
    if (glm::any(glm::isnan(velocity))) resetVelocity();
    lastVelocity = Util::lerpV(lastVelocity, velocity, SM::delta * lerpAcceleration);
    pos += lastVelocity * SM::delta;
}

void Boid::resetVelocity() {
    velocity = normalize(Util::randomv(-5, 5));
}
