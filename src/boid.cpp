#include "boid.h"
using namespace BoidInfo;

void Boid::process(std::deque<Boid*> boids, std::vector<vec3> homes) {
    dir = normalize(velocity);
    move(boids, homes);
    limitSpeed();
    update();
}

void Boid::move(std::deque<Boid*> boids, std::vector<vec3> homes) {
    int numNeighbors = boids.size() - 1;

    // only move by family rules if near family. otherwise, boid will move towards origin due to
    // subtraction in alignment and cohesion checks
    int numFamily = 0;

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

    for (Boid* otherBoid : boids) {
        if (otherBoid->ID != ID) {
            float distFromBoid = distance(pos, otherBoid->pos);

            if (distFromBoid < family_range) {
                // stay within group of same boid type
                if (isFamily(type, otherBoid->type)) {
                    numFamily++;

                    // alignment
                    avgVel += otherBoid->velocity;

                    // cohesion
                    avgCentre += otherBoid->pos;

                    // separation
                    if (distFromBoid < minSepDistance) {
                        avgMove += pos - otherBoid->pos;
                    }
                } else if (!SM::canBoidsAttack) {
                    // to avoid altering if-statement structure in case i decide to remove this
                } else {
                    if (isPreyTo(type, otherBoid->type)) {
                        if (distFromBoid <= minEnemyInterceptDistance) {
                            isBeingChased = true;
                            // printf("CHASED\n");
                        }
                        avgMoveFlee += (pos - otherBoid->pos) * fearWeight;
                    } else if (isPredatorTo(type, otherBoid->type)) {
                        isInPursuit = isInPursuit || distFromBoid <= minEnemyInterceptDistance;
                        if (distFromBoid <= minEnemyChaseDistance) {
                            // move towards goal
                            isInChasing = true;
                            if (distFromBoid < closestPreyDist) {
                                closestPrey = otherBoid->pos;
                                closestPreyDist = distFromBoid;
                            }
                            avgMoveAtt -= (pos - otherBoid->pos) * goalWeight;
                        } else if (distFromBoid <= minEnemyInterceptDistance) {
                            // intercept goal
                            // doing it like this means predators are drawn towards larger groups more than single prey
                            avgMoveAtt -= (pos - (otherBoid->pos + otherBoid->dir * otherBoid->speed)) * goalWeight;
                        }
                    }
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
                    Util::printVec3(home);
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
            velocity += (avgVel - velocity) * matchingFactor;

            // cohesion
            avgCentre /= numFamily;
            velocity += (avgCentre - pos) * centeringFactor;
        }

        // separation
        if (isBeingChased) {
            if (hasHome) {
                avgMoveFlee -= (pos - currentHome) * fearWeight * .5f;  // fleeing home
            }
            velocity += avgMoveFlee * avoidFactor;
        } else if (isInPursuit) {
            if (isInChasing) {
                // chase closest target only
                avgMoveAtt = -(pos - closestPrey) * goalWeight;
            }
            velocity += avgMoveAtt * avoidFactor;
        }
        if (hasHome) {
            velocity -= (pos - currentHome) * 0.005f; // drift towards home
        }
        velocity += avgMove * avoidFactor;
    }
}

void Boid::limitSpeed() {
    float tspeed = dot(velocity, velocity);
    // can use glsl's isnan in a compute shader, so this is fine: https://registry.khronos.org/OpenGL-Refpages/gl4/html/isnan.xhtml
    if (tspeed < min_speed * min_speed || glm::all(glm::isnan(velocity))) {
        velocity = vec3(min_speed);
    } else {
        float max_speed = getBoidMaxSpeed(type);
        if (tspeed > max_speed * max_speed) {
            velocity = normalize(velocity) * max_speed;
        }
    }
}

void Boid::update() {
    float tf = 1;
    if (pos.x < SM::WORLD_BOUND_LOW)
        velocity.x += tf;
    if (pos.x > SM::WORLD_BOUND_HIGH)
        velocity.x -= tf;
    if (pos.y < SM::WORLD_BOUND_LOW)
        velocity.y += tf;
    if (pos.y > SM::WORLD_BOUND_HIGH)
        velocity.y -= tf;
    if (pos.z < SM::WORLD_BOUND_LOW)
        velocity.z += tf;
    if (pos.z > SM::WORLD_BOUND_HIGH)
        velocity.z -= tf;
    lastVelocity = Util::lerpV(lastVelocity, velocity, SM::delta * lerpAcceleration);
    pos += lastVelocity * SM::delta;
}
