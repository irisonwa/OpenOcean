#ifndef SM_H
#define SM_H
#include <windows.h>
#include <iostream>
#include "util.h"
// #include "camera.h"

// Scene Manager singleton
namespace SM {
    enum CAMERA_MODE {
        FIRST = 0,
        THIRD = 1,
        FREE = 2
    };

    // Update the global delta value on each frame.
    extern void updateDelta();
    // Update the delta mouse positions
    extern void updateMouse(int nx, int ny);
    extern void activateFreeCam();
    extern void activateFirstPerson();
    extern void activateThirdPerson();
    extern void changeCameraState();

    // delta time
    extern float delta;

    // start time of the program
    extern DWORD startTime;

    extern const float floor_position;
    extern bool flashlightToggled;

    extern int width; // screen width
    extern int height; // screen height

    extern float WORLD_BOUND_HIGH;
    extern float WORLD_BOUND_LOW;

    extern int unnamedMeshCount;
    extern int unnamedBoneMeshCount;
    
    extern float mouseDX; // mouse delta x
    extern float mouseDY; // mouse delta y

    extern bool isFreeCam;
    extern bool isFirstPerson;
    extern bool isThirdPerson;
    // extern Camera activeCamera;
    extern CAMERA_MODE camMode;
    extern bool showNormal;

    extern int MAX_NUM_BOIDS; // maximum count of boids allowed to be rendered. 

};

#endif /* SM_H */
