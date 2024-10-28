#ifndef BOID_H
#define BOID_H

#include "util.h"
#include "sm.h"
#include <deque>
using namespace glm;

enum BoidType {
    PRED_1,
    PRED_2,
    FISH_1,
    FISH_2,
    FISH_3,
    PLANKTON
};

class Boid {
   private:
    /* data */
   public:
    Boid(vec3 _pos, vec3 vel, BoidType t, int id);
    ~Boid();
    void process(std::deque<Boid*>);
    void move(std::deque<Boid*>);
    void limitSpeed();
    void update();

    const float FAMILY_RANGE = 10; /* the distance the boid will check for other boids */
    const float MAX_SPEED = 20;   /* the maximum speed of the boid */
    // static final int MAX_CHASE_SPEED = 800; /* the maximum speed of the boid */
    const float originalViewCone = 180; /* view angle */

    vec3 pos;                          /* position */
    vec3 velocity;                     /* current velocity of boid */
    vec3 dir;                          /* current direction of boid; always equal to normalised velocity */
    vec3 lastVelocity;                 /* last velocity of boid, before movement transformations */
    vec3 home;                         /* location of safe area */
    bool updateHistory = false;        /* update the history of this boid? (for drawing trails) */
    bool isCaught = false;             /* was this boid caught by a predator? :( */
    bool inPursuit = false;            /* is the boid moving towards something? */
    bool isBeingChased = false;        /* is this boid being chased? */
    bool wasBeingChased = false;       /* was this boid being chased? used for remaining panicked */
    float viewCone = originalViewCone; /* view angle */
    float speed = MAX_SPEED;           /* current speed of the boid */
    float acceleration;                /* coming soon */
    // std::vector<std::vector<vec3>> history;
    int ID; /* ID of boid [can be replaced by with glInstance_ID in GLSL] */
    BoidType type;
};


#endif /* BOID_H */
