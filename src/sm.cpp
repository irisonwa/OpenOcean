#include "sm.h"
#include <iostream>

namespace SM {
    int width = 1024;
    int height = 600;
    const float floor_position = 0.f;
    bool flashlightToggled = false;
    inline float delta = 0.0f;
    DWORD startTime = 0;
    int unnamedMeshCount = 0;
    int unnamedBoneMeshCount = 0;

    void updateDelta() {
        static DWORD last_time = 0;
        DWORD curr_time = timeGetTime();
        if (last_time == 0) last_time = curr_time;
        delta = (curr_time - last_time) * 0.001f;
        last_time = curr_time;
    }
}
