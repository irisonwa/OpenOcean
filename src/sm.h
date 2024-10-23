#ifndef SM_H
#define SM_H
#include <windows.h>

namespace SM {
    /// <summary>
    /// Update the global delta value on each frame.
    /// </summary>
    extern void updateDelta();

    /// <summary>
    /// delta time
    /// </summary>
    extern float delta;

    extern DWORD startTime;

    extern const float floor_position;
    extern bool flashlightToggled;

    extern int width;
    extern int height;

    extern int unnamedMeshCount;
    extern int unnamedBoneMeshCount;
};

#endif /* SM_H */
