#ifndef BOIDINFO_H
#define BOIDINFO_H

#include <set>
#include <map>
#include "util.h"

class Boid;
class Flock;

#define NUM_BOID_TYPES 12

// boid struct. smaller representation of a Boid object. used for compute shaders
struct BoidS {
    vec4 pos;
    vec4 velocity;
    vec4 lastVelocity;
    vec4 dir;
    vec4 scale;
    vec4 home;
    vec2 bounds;
    float min_speed;
    float max_speed;
    float minSepDistance;
    float matchingFactor;
    float centeringFactor;
    float avoidFactor;
    float minEnemyInterceptDistance;
    float minEnemyChaseDistance;
    float fearWeight;
    float goalWeight;
    int canHaveHome;
    int hasHome;
    int isBeingChased;
    int isChasing;
    float boidsAround;
    unsigned int type;
    unsigned int ID;
    unsigned int myPredators[NUM_BOID_TYPES];
    unsigned int myPrey[NUM_BOID_TYPES];
    int pd1;
    // int pd2;
    // int pd3;
};

enum BoidType {
    F_THREADFIN,
    F_MARLIN,
    F_SPEAR_FISH,
    F_TUNA,
    F_HERRING,
    F_CLOWNFISH,
    S_BLUE,
    S_WHALE,
    S_WHITE,
    WHALE,
    DOLPHIN,
    PLANKTON,
};

namespace BoidInfo {
    extern std::map<BoidType, std::set<BoidType>> preyTable; // table for each boid's prey
    extern std::map<BoidType, std::set<BoidType>> predTable; // table for each boid's predators
    extern vec3 getBoidScale(BoidType t);
    extern float getBoidMinSpeed(BoidType t);
    extern float getBoidMaxSpeed(BoidType t);
    extern float getBoidSepDistance(BoidType t);
    extern float getBoidChaseDistance(BoidType t);
    extern float getBoidInterceptDistance(BoidType t);
    extern float getBoidAvoidFactor(BoidType t);
    extern float getBoidGoalWeight(BoidType t);
    extern float getBoidFearWeight(BoidType a, BoidType b);
    extern float getBoidMatchingFactor(BoidType t);
    extern float getBoidCenteringFactor(BoidType t);
    extern vec2 getBoidBounds(BoidType t);
    extern int getHomeValidation(BoidType t);
    extern bool isFamily(BoidType a, BoidType b);
    extern bool isPreyTo(BoidType a, BoidType b);
    extern bool isPredatorTo(BoidType a, BoidType b);
    extern BoidS createBoidStruct(BoidType t, unsigned id, vec3 pos, vec3 dirs);
    extern std::string getBoidName(BoidType t);
} // namespace BoidInfo


#endif /* BOIDINFO_H */
