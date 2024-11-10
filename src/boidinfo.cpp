#include "boidinfo.h"
using namespace glm;

namespace BoidInfo {
    std::map<BoidType, std::set<BoidType>> preyTable = {
        {F_THREADFIN, {
            PLANKTON
        }},
        {F_BLACK_MARLIN, {
            
        }},
        {F_STRIPED_MARLIN, {
            
        }},
        {F_SPEAR_FISH, {
            
        }},
        {F_TUNA, {
            
        }},
        {F_HERRING, {
            
        }},
        {S_BLUE, {
            F_THREADFIN,
            F_BLACK_MARLIN,
            F_STRIPED_MARLIN,
            F_SPEAR_FISH,
            F_TUNA,
            F_HERRING
        }},
        {S_WHALE, {
            
        }},
        {S_WHITE, {
            
        }},
        {WHALE, {
            
        }},
        {DOLPHIN, {
            
        }},
        {PLANKTON, {
            
        }}
    };
    
    std::map<BoidType, std::set<BoidType>> predTable = {
        {F_THREADFIN, {
            S_BLUE,
            S_WHALE,
            S_WHITE,
            WHALE
        }},
        {F_BLACK_MARLIN, {
            
        }},
        {F_STRIPED_MARLIN, {
            
        }},
        {F_SPEAR_FISH, {
            
        }},
        {F_TUNA, {
            
        }},
        {F_HERRING, {
            
        }},
        {S_BLUE, {
            
        }},
        {S_WHALE, {
            
        }},
        {S_WHITE, {
            
        }},
        {WHALE, {
            
        }},
        {DOLPHIN, {
            
        }},
        {PLANKTON, {
            
        }}
    };

    vec3 getBoidScale(BoidType t) {
        switch (t) {
            case F_THREADFIN:
                return vec3(.5);
            case F_BLACK_MARLIN:
                return vec3(1);
            case F_STRIPED_MARLIN:
                return vec3(1);
            case F_SPEAR_FISH:
                return vec3(1);
            case F_TUNA:
                return vec3(1);
            case F_HERRING:
                return vec3(1);
            case S_BLUE:
                return vec3(1);
            case S_WHALE:
                return vec3(1);
            case S_WHITE:
                return vec3(1);
            case WHALE:
                return vec3(1);
            case DOLPHIN:
                return vec3(1);
            case PLANKTON:
                return vec3(1);
            default:
                return vec3(1);
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
} // namespace BoidInfo
