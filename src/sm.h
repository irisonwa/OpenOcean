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

    extern int unnamedMeshCount;
    extern int unnamedBoneMeshCount;

    extern glm::vec3 UP;  // World up
    extern glm::vec3 FORWARD;  // World forward
    extern glm::vec3 RIGHT;  // World right
    extern glm::vec3 X;  // Value of 1 on x axis
    extern glm::vec3 Y;  // Value of 1 on y axis
    extern glm::vec3 Z;  // Value of 1 on z axis
    
    extern float mouseDX; // mouse delta x
    extern float mouseDY; // mouse delta y

    extern bool isFreeCam;
    extern bool isFirstPerson;
    extern bool isThirdPerson;
    // extern Camera activeCamera;
    extern CAMERA_MODE camMode;

};

#endif /* SM_H */
