#include "boidinfo.h"
using namespace glm;

namespace BoidInfo {
std::map<BoidType, std::set<BoidType>> preyTable = {
    {F_THREADFIN, {PLANKTON}},
    {F_MARLIN, {PLANKTON}},
    {F_SPEAR_FISH, {PLANKTON}},
    {F_TUNA, {PLANKTON}},
    {F_HERRING, {PLANKTON}},
    {F_CLOWNFISH, {PLANKTON}},
    {S_BLUE, {F_THREADFIN, F_MARLIN, F_SPEAR_FISH, F_TUNA, F_HERRING, F_CLOWNFISH}},
    {S_WHALE, {}},
    {S_WHITE, {}},
    {DOLPHIN, {}},
    {WHALE, {}},
    {PLANKTON, {}}};

std::map<BoidType, std::set<BoidType>> predTable = {
    {F_THREADFIN, {S_BLUE, S_WHALE, S_WHITE, WHALE}},
    {F_MARLIN, {S_BLUE, S_WHALE, S_WHITE, WHALE}},
    {F_SPEAR_FISH, {S_BLUE, S_WHALE, S_WHITE, WHALE}},
    {F_TUNA, {S_BLUE, S_WHALE, S_WHITE, WHALE}},
    {F_HERRING, {S_BLUE, S_WHALE, S_WHITE, WHALE}},
    {F_CLOWNFISH, {S_BLUE, S_WHALE, S_WHITE, WHALE}},
    {S_BLUE, {WHALE}},
    {S_WHALE, {WHALE}},
    {S_WHITE, {WHALE}},
    {DOLPHIN, {WHALE}},
    {WHALE, {}},
    {PLANKTON, {F_THREADFIN, F_MARLIN, F_SPEAR_FISH, F_TUNA, F_HERRING, F_CLOWNFISH, S_BLUE, S_WHALE, S_WHITE, WHALE, DOLPHIN}}};

vec3 getBoidScale(BoidType t) {
    switch (t) {
        case F_THREADFIN:
            return vec3(.25);
        case F_MARLIN:
            return vec3(.5);
        case F_SPEAR_FISH:
            return vec3(.5);
        case F_TUNA:
            return vec3(1);
        case F_HERRING:
            return vec3(1);
        case F_CLOWNFISH:
            return vec3(1);
        case S_BLUE:
            return vec3(1);
        case S_WHALE:
            return vec3(1);
        case S_WHITE:
            return vec3(1);
        case WHALE:
            return vec3(3);
        case DOLPHIN:
            return vec3(1);
        case PLANKTON:
            return vec3(.1);
        default:
            return vec3(1);
    }
}

float getBoidMinSpeed(BoidType t) {
    return 1;
}

float getBoidMaxSpeed(BoidType t) {
    switch (t) {
        case F_THREADFIN:
            return 11;
        case F_MARLIN:
            return 8;
        case F_SPEAR_FISH:
            return 8;
        case F_TUNA:
            return 10;
        case F_HERRING:
            return 11;
        case F_CLOWNFISH:
            return 11;
        case S_BLUE:
            return 10;
        case S_WHALE:
            return 10;
        case S_WHITE:
            return 10;
        case WHALE:
            return 1;
        case DOLPHIN:
            return 14;
        case PLANKTON:
            return 4;
        default:
            return 10;
    }
}
float getBoidSepDistance(BoidType t) {
    switch (t) {
        case F_THREADFIN:
            return 0.2;
        case F_MARLIN:
            return 0.4;
        case F_SPEAR_FISH:
            return 0.4;
        case F_TUNA:
            return 0.4;
        case F_HERRING:
            return 0.2;
        case F_CLOWNFISH:
            return 0.2;
        case S_BLUE:
            return 0.5;
        case S_WHALE:
            return 0.5;
        case S_WHITE:
            return 0.5;
        case WHALE:
            return 1.5;
        case DOLPHIN:
            return 0.5;
        case PLANKTON:
            return 0.1;
        default:
            return 0.5;
    }
}
float getBoidChaseDistance(BoidType t) {
    switch (t) {
        case F_THREADFIN:
            return 1;
        case F_MARLIN:
            return 1;
        case F_SPEAR_FISH:
            return 1;
        case F_TUNA:
            return 1;
        case F_HERRING:
            return 1;
        case F_CLOWNFISH:
            return 1;
        case S_BLUE:
            return 1.5;
        case S_WHALE:
            return 1.5;
        case S_WHITE:
            return 1.5;
        case WHALE:
            return 1.5;
        case DOLPHIN:
            return 1.5;
        case PLANKTON:
            return 0.5;
        default:
            return 1.5;
    }
}
float getBoidInterceptDistance(BoidType t) {
    switch (t) {
        case F_THREADFIN:
            return 4;
        case F_MARLIN:
            return 4;
        case F_SPEAR_FISH:
            return 4;
        case F_TUNA:
            return 4;
        case F_HERRING:
            return 4;
        case F_CLOWNFISH:
            return 4;
        case S_BLUE:
            return 8;
        case S_WHALE:
            return 8;
        case S_WHITE:
            return 8;
        case WHALE:
            return 1.5; // same as chase distance
        case DOLPHIN:
            return 8;
        case PLANKTON:
            return 1;
        default:
            return 1;
    }
}
float getBoidAvoidFactor(BoidType t) {
    switch (t) {
        case F_THREADFIN:
            return 0.1;
        case F_MARLIN:
            return 0.1;
        case F_SPEAR_FISH:
            return 0.1;
        case F_TUNA:
            return 0.1;
        case F_HERRING:
            return 0.1;
        case F_CLOWNFISH:
            return 0.1;
        case S_BLUE:
            return 0.1;
        case S_WHALE:
            return 0.1;
        case S_WHITE:
            return 0.1;
        case WHALE:
            return 0.1;
        case DOLPHIN:
            return 0.1;
        case PLANKTON:
            return 0.1;
        default:
            return 0.1;
    }
}
float getBoidGoalWeight(BoidType t) {
    switch (t) {
        case F_THREADFIN:
            return 0.1;
        case F_MARLIN:
            return 0.1;
        case F_SPEAR_FISH:
            return 0.1;
        case F_TUNA:
            return 0.1;
        case F_HERRING:
            return 0.1;
        case F_CLOWNFISH:
            return 0.1;
        case S_BLUE:
            return 0.2;
        case S_WHALE:
            return 0.1;
        case S_WHITE:
            return 0.1;
        case WHALE:
            return 0.1;
        case DOLPHIN:
            return 0.1;
        case PLANKTON:
            return 0.1;
        default:
            return 0.1;
    }
}

float getBoidFearWeight(BoidType a, BoidType b) {
    if (isPreyTo(a, b)) {
        return 5;
    } else if (isPredatorTo(a, b) || isFamily(a, b)) {
        return 0;
    } else {
        return max(((getBoidScale(b) / getBoidScale(a)) - getBoidScale(a)).x, 0.f);
    }
}

float getBoidMatchingFactor(BoidType t) {
    return 0.05f;
}
float getBoidCenteringFactor(BoidType t) {
    return 0.005f;
}

vec2 getBoidBounds(BoidType t) {
    switch (t) {
        case F_THREADFIN:
            return {SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH};
        case F_MARLIN:
            return {SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH};
        case F_SPEAR_FISH:
            return {SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH};
        case F_TUNA:
            return {SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH};
        case F_HERRING:
            return {SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH};
        case F_CLOWNFISH:
            return {SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH};
        case S_BLUE:
            return {SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH};
        case S_WHALE:
            return {SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH};
        case S_WHITE:
            return {SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH};
        case WHALE:
            return {SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH};
        case DOLPHIN:
            return {SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH};
        case PLANKTON:
            return {SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH};
        default:
            return {SM::WORLD_BOUND_LOW, SM::WORLD_BOUND_HIGH};
    }
}

// Can this boid type have a home?
int getHomeValidation(BoidType t) {
    return 1;
    switch (t) {
        case F_THREADFIN:
            return true;
        case F_MARLIN:
            return false;
        case F_SPEAR_FISH:
            return false;
        case F_TUNA:
            return false;
        case F_HERRING:
            return true;
        case F_CLOWNFISH:
            return true;
        case S_BLUE:
            return false;
        case S_WHALE:
            return false;
        case S_WHITE:
            return false;
        case WHALE:
            return false;
        case DOLPHIN:
            return false;
        case PLANKTON:
            return false;
        default:
            return false;
    }
}

bool isFamily(BoidType a, BoidType b) {
    return a == b;
}

// Is `a` prey to `b`?
bool isPreyTo(BoidType a, BoidType b) {
    return predTable[a].contains(b);
}

// Is `a` a predator to `b`?
bool isPredatorTo(BoidType a, BoidType b) {
    return preyTable[a].contains(b);
}

BoidS createBoidStruct(BoidType t, unsigned id, vec3 pos, vec3 dirs) {
    BoidS b = BoidS();
    b.ID = id;
    b.pos = vec4(pos, 1);
    b.velocity = b.lastVelocity = vec4(dirs, 1);
    b.type = t;
    b.min_speed = getBoidMinSpeed(t);
    b.max_speed = getBoidMaxSpeed(t);
    b.minSepDistance = getBoidSepDistance(t);
    b.minEnemyChaseDistance = getBoidChaseDistance(t);
    b.minEnemyInterceptDistance = getBoidInterceptDistance(t);
    b.avoidFactor = getBoidAvoidFactor(t);
    b.goalWeight = getBoidGoalWeight(t);
    b.matchingFactor = getBoidMatchingFactor(t);
    b.centeringFactor = getBoidCenteringFactor(t);
    b.bounds = getBoidBounds(t);
    b.canHaveHome = getHomeValidation(t);
    b.hasHome = false;
    for (int i = 0; i < 12; ++i) {
        if (isPreyTo(t, (BoidType)i)) b.myPredators[i] = 1;
        else b.myPredators[i] = 0;
        if (isPredatorTo(t, (BoidType)i)) b.myPrey[i] = 1;
        else b.myPrey[i] = 0;
    }
    return b;
}
}  // namespace BoidInfo
