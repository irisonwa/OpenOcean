#ifndef BOIDINFO_H
#define BOIDINFO_H

#include <set>
#include <map>
#include "util.h"

class Boid;
class Flock;

enum BoidType {
    F_THREADFIN,
    // F_BLACK_MARLIN,
    // F_STRIPED_MARLIN,
    F_MARLIN,
    F_SPEAR_FISH,
    F_TUNA,
    F_HERRING,
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
    extern bool isFamily(BoidType a, BoidType b);
    extern bool isPreyTo(BoidType a, BoidType b);
    extern bool isPredatorTo(BoidType a, BoidType b);
} // namespace BoidInfo


#endif /* BOIDINFO_H */
