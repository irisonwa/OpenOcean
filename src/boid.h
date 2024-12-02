#ifndef BOID_H
#define BOID_H

#include <vector>
#include <deque>

#include "boidinfo.h"
#include "util.h"
#include "sm.h"
#include "variantmesh.h"
using namespace glm;

// Container struct for boids. Since the octree will need to access the boids, I don't want each subtree having a copy of the boid list.
// It was either this or keeping the boids in the Scene Manager, which seemed stupid.
struct BoidContainer {
    Boid** boids;
    unsigned size;
};

class Boid {
   private:
    float lerpAcceleration = 8; /* how fast to lerp velocity */
   public:
    Boid(vec3 _pos, vec3 vel, BoidType t, int id) {
        pos = _pos;
        velocity = vel;
        vec3 nVel = normalize(velocity);
        dir = nVel;
        velocity = nVel;
        lastVelocity = nVel;
        type = t;
        ID = id;
    }

    ~Boid() {}

    void process(BoidContainer*&, const unsigned*, int, std::vector<vec3> homes);
    void move(BoidContainer*&, const unsigned*, int, std::vector<vec3> homes);
    void limitSpeed();
    void update();
    void resetVelocity();

    float visibleRange = 8; /* the distance the boid will check for other boids */
    // float max_speed = 10;    /* the maximum speed of the boid */
    // float min_speed = 1;    /* the maximum speed of the boid */
    // // static final int MAX_CHASE_SPEED = 800; /* the maximum speed of the boid */
    const float originalViewCone = 180; /* view angle */
    // float minSepDistance = .5;          // minimum distance between boids before they start moving away
    // float matchingFactor = 0.05;        // how much to match velocity
    // float centeringFactor = 0.005;      // how much to match position
    // float avoidFactor = 0.1;            // how much to avoid when closer than minSepDistance
    // float minEnemyInterceptDistance = 8;
    // float minEnemyChaseDistance = 1.5;
    // float fearWeight = 5;
    // float goalWeight = .1;
    vec3 currentHome = vec3(0, 0, 0); /* location of safe area */
    int canHaveHome = 0;
    int hasHome = 0;
    float newHomeDistDrift = 20;  // distance to determine new home when drifting (i.e., not fleeing or chasing)
    float newHomeDistFlee = 10;   // distance to determine new home when fleeing
    float homeRange = 40;         // distance to consider current home valid

    vec3 pos;                          /* position */
    vec3 velocity;                     /* current velocity of boid */
    vec3 dir;                          /* current direction of boid; always equal to normalised velocity */
    vec3 lastVelocity;                 /* last velocity of boid, before movement transformations. used for lerping */
    bool isCaught = false;             /* was this boid caught by a predator? :( */
    float viewCone = originalViewCone; /* view angle */
    unsigned ID;                       /* ID of boid */
    BoidType type;
    // bool updateHistory = false;        /* update the history of this boid? (for drawing trails) */
    // std::vector<std::vector<vec3>> history;
};

#endif /* BOID_H */
