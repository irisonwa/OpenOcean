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

    float visibleRange = 8;           /* the distance the boid will check for other boids */
    vec3 currentHome = vec3(0, 0, 0); /* location of safe area */
    int canHaveHome = 0;
    int hasHome = 0;
    float newHomeDistDrift = 20;  // distance to determine new home when drifting (i.e., not fleeing or chasing)
    float newHomeDistFlee = 10;   // distance to determine new home when fleeing
    float homeRange = 40;         // distance to consider current home valid

    vec3 pos;              /* position */
    vec3 velocity;         /* current velocity of boid */
    vec3 dir;              /* current direction of boid; always equal to normalised velocity */
    vec3 lastVelocity;     /* last velocity of boid, before movement transformations. used for lerping */
    bool isCaught = false; /* was this boid caught by a predator? :( */
    unsigned ID;           /* ID of boid */
    BoidType type;
};

#endif /* BOID_H */
