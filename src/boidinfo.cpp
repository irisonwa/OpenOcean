#include "boidinfo.h"
using namespace glm;

namespace BoidInfo {
std::map<BoidType, std::set<BoidType>> preyTable = {
    {F_THREADFIN, {PLANKTON}},
    {F_MARLIN, {PLANKTON}},
    {F_SPEAR_FISH, {PLANKTON}},
    {F_HERRING, {PLANKTON}},
    {F_CLOWNFISH, {PLANKTON}},
    {S_BLUE, {PLANKTON, F_THREADFIN, F_MARLIN, F_SPEAR_FISH, F_HERRING, F_CLOWNFISH}},
    {S_WHALE, {PLANKTON, F_HERRING}},
    {S_WHITE, {PLANKTON, F_THREADFIN, F_MARLIN, F_SPEAR_FISH, F_HERRING, F_CLOWNFISH, DOLPHIN}},
    {DOLPHIN, {PLANKTON, F_THREADFIN, F_MARLIN, F_SPEAR_FISH, F_HERRING, F_CLOWNFISH}},
    {WHALE, {PLANKTON, F_HERRING}},
    {PLANKTON, {}}};

std::map<BoidType, std::set<BoidType>> predTable = {
    {F_THREADFIN, {DOLPHIN, S_BLUE, S_WHITE, WHALE}},
    {F_MARLIN, {DOLPHIN, S_BLUE, S_WHITE, WHALE}},
    {F_SPEAR_FISH, {DOLPHIN, S_BLUE, S_WHITE, WHALE}},
    {F_HERRING, {DOLPHIN, S_BLUE, S_WHALE, S_WHITE, WHALE}},
    {F_CLOWNFISH, {DOLPHIN, S_BLUE, S_WHALE, S_WHITE, WHALE}},
    {S_BLUE, {}},
    {S_WHALE, {}},
    {S_WHITE, {WHALE}}, // scared for no reason (dumb idiot)
    {DOLPHIN, {}},
    {WHALE, {}},
    {PLANKTON, {F_THREADFIN, F_MARLIN, F_SPEAR_FISH, F_HERRING, F_CLOWNFISH, S_BLUE, S_WHALE, S_WHITE, WHALE, DOLPHIN}}};

vec3 getBoidScale(BoidType t) {
    switch (t) {
        case F_THREADFIN:
            return vec3(.125);
        case F_MARLIN:
            return vec3(.5);
        case F_SPEAR_FISH:
            return vec3(.5);
        case F_HERRING:
            return vec3(.1);
        case F_CLOWNFISH:
            return vec3(.1);
        case S_BLUE:
            return vec3(1);
        case S_WHALE:
            return vec3(1.5);
        case S_WHITE:
            return vec3(1);
        case WHALE:
            return vec3(2);
        case DOLPHIN:
            return vec3(0.5);
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
    float m = getBoidMinSpeed(t)/32;
    switch (t) {
        case F_THREADFIN:
            return 1 + (8 * m);
        case F_MARLIN:
            return 1 + (8 * m);
        case F_SPEAR_FISH:
            return 1 + (8 * m);
        case F_HERRING:
            return 1 + (8 * m);
        case F_CLOWNFISH:
            return 1 + (8 * m);
        case S_BLUE:
            return 1 + (12 * m);
        case S_WHALE:
            return 1 + (12 * m);
        case S_WHITE:
            return 1 + (12 * m);
        case WHALE:
            return 1 + (2 * m);
        case DOLPHIN:
            return 1 + (32 * m);
        case PLANKTON:
            return 1 + (4 * m);
        default:
            return 1 + (10 * m);
    }
}
float getBoidSepDistance(BoidType t) {
    switch (t) {
        case F_THREADFIN:
            return getBoidScale(t).x * 2;
        case F_MARLIN:
            return getBoidScale(t).x * 2;
        case F_SPEAR_FISH:
            return getBoidScale(t).x * 2;
        case F_HERRING:
            return getBoidScale(t).x * 2;
        case F_CLOWNFISH:
            return getBoidScale(t).x * 2;
        case S_BLUE:
            return getBoidScale(t).x * 2;
        case S_WHALE:
            return getBoidScale(t).x * 2;
        case S_WHITE:
            return getBoidScale(t).x * 2;
        case WHALE:
            return getBoidScale(t).x * 2;
        case DOLPHIN:
            return getBoidScale(t).x * 2;
        case PLANKTON:
            return getBoidScale(t).x * 2;
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
        case F_HERRING:
            return 1;
        case F_CLOWNFISH:
            return 1;
        case S_BLUE:
            return 1.5;
        case S_WHALE:
            return 2;
        case S_WHITE:
            return 2;
        case WHALE:
            return 2;
        case DOLPHIN:
            return 2;
        case PLANKTON:
            return 0.5;
        default:
            return 2;
    }
}
float getBoidInterceptDistance(BoidType t) {
    switch (t) {
        case F_THREADFIN:
            return 8;
        case F_MARLIN:
            return 8;
        case F_SPEAR_FISH:
            return 8;
        case F_HERRING:
            return 8;
        case F_CLOWNFISH:
            return 8;
        case S_BLUE:
            return 8;
        case S_WHALE:
            return 8;
        case S_WHITE:
            return 8;
        case WHALE:
            return getBoidChaseDistance(t);
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
        case F_HERRING:
            return 0.1;
        case F_CLOWNFISH:
            return 0.1;
        case S_BLUE:
            return 0.2;
        case S_WHALE:
            return 0.1;
        case S_WHITE:
            return 0.4; // more aggressive
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

float getBoidMatchingFactor(BoidType t) {
    return 0.05f;
}
float getBoidCenteringFactor(BoidType t) {
    return 0.005f;
}

vec2 getBoidBounds(BoidType t) {
    switch (t) {
        case F_THREADFIN:
            return {WORLD_BOUND_LOW, WORLD_BOUND_HIGH};
        case F_MARLIN:
            return {WORLD_BOUND_LOW, WORLD_BOUND_HIGH};
        case F_SPEAR_FISH:
            return {WORLD_BOUND_LOW, WORLD_BOUND_HIGH};
        case F_HERRING:
            return {WORLD_BOUND_LOW, WORLD_BOUND_HIGH};
        case F_CLOWNFISH:
            return {WORLD_BOUND_LOW, WORLD_BOUND_HIGH};
        case S_BLUE:
            // todo: lower down
            return {WORLD_BOUND_LOW, WORLD_BOUND_HIGH};
        case S_WHALE:
            return {WORLD_BOUND_LOW, WORLD_BOUND_HIGH};
        case S_WHITE:
            // todo: higher up
            return {WORLD_BOUND_LOW, WORLD_BOUND_HIGH};
        case WHALE:
            return {WORLD_BOUND_LOW, WORLD_BOUND_HIGH};
        case DOLPHIN:
            return {WORLD_BOUND_LOW, WORLD_BOUND_HIGH};
        case PLANKTON:
            return {WORLD_BOUND_LOW, WORLD_BOUND_HIGH};
        default:
            return {WORLD_BOUND_LOW, WORLD_BOUND_HIGH};
    }
}

// Can this boid type have a home?
int getHomeValidation(BoidType t) {
    switch (t) {
        case F_THREADFIN:
        case F_HERRING:
        case F_CLOWNFISH:
        case PLANKTON:
            return true;
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

float getBoidFearWeight(BoidType a, BoidType b) {
    if (isPreyTo(a, b)) {
        return 5;
    } else if (isPredatorTo(a, b) || isFamily(a, b)) {
        return 0;
    } else {
        return max(((getBoidScale(b) / getBoidScale(a)) - getBoidScale(a)).x, 0.f);
    }
}

BoidS createBoidStruct(BoidType t, unsigned id, vec3 pos, vec3 vel) {
    BoidS b = BoidS();
    b.ID = id;
    b.pos = vec4(pos, 0);
    b.velocity = vec4(vel, 0);
    b.lastVelocity = vec4(vel, 0);
    b.dir = normalize(vec4(vel, 0));
    b.type = t;
    b.scale = vec4(getBoidScale(t), 0);
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
    b.hasHome = 0;
    b.home = vec4(1e9);
    b.isBeingChased = false;
    b.isChasing = false;
    b.boidsAround = -1;
    for (int i = 0; i < NUM_BOID_TYPES; ++i) {
        if (isPreyTo(t, (BoidType)i)) b.myPredators[i] = 1;
        else b.myPredators[i] = 0;
        if (isPredatorTo(t, (BoidType)i)) b.myPrey[i] = 1;
        else b.myPrey[i] = 0;
    }
    return b;
}

std::string getBoidName(BoidType t) {
    switch (t) {
        case F_THREADFIN:
            return "Threadfin";
        case F_MARLIN:
            return "Marlin";
        case F_SPEAR_FISH:
            return "Spear Fish";
        case F_HERRING:
            return "Herring";
        case F_CLOWNFISH:
            return "Clownfish";
        case S_BLUE:
            return "Blue Shark";
        case S_WHALE:
            return "Whale Shark";
        case S_WHITE:
            return "White Shark";
        case WHALE:
            return "Whale";
        case DOLPHIN:
            return "Dolphin";
        case PLANKTON:
            return "Plankton";
        default:
            return "_GENERIC_BOID_";
    }
}
}  // namespace BoidInfo
