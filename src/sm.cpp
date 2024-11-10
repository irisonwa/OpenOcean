#include "sm.h"

namespace SM {
    int width = 1024;
    int height = 600;

    float WORLD_BOUND_HIGH = 50;
    float WORLD_BOUND_LOW = -50;
    
    const float floor_position = 0.f;
    bool flashlightToggled = false;
    
    inline float delta = 0.0f;
    DWORD startTime = 0;
    
    int unnamedMeshCount = 0;
    int unnamedBoneMeshCount = 0;
    int unnamedVariantMeshCount = 0;

    float mouseDX = 0;
    float mouseDY = 0;

    bool isFreeCam = false;
    bool isFirstPerson = false;
    bool isThirdPerson = true;
    // Camera activeCamera = Camera();
    CAMERA_MODE camMode = CAMERA_MODE::THIRD;

    bool showNormal = false;

    int MAX_NUM_BOIDS = 10000;
    bool canBoidsAttack = true;

    void updateDelta() {
        static DWORD last_time = 0;
        DWORD curr_time = timeGetTime();
        if (last_time == 0) last_time = curr_time;
        delta = (curr_time - last_time) * 0.001f;
        last_time = curr_time;
    }
    
    void updateMouse(int nx, int ny) {
        float xPos = width / 2.0;
        float yPos = height / 2.0;
        mouseDX = nx - xPos;
        mouseDY = ny - yPos;
        glutWarpPointer(xPos, yPos);
    }

    void activateFreeCam() {
        isFreeCam = true;
        isFirstPerson = false;
        isThirdPerson = false;
    }
    void activateFirstPerson() {
        isFreeCam = false;
        isFirstPerson = true;
        isThirdPerson = false;
    }
    void activateThirdPerson() {
        isFreeCam = false;
        isFirstPerson = false;
        isThirdPerson = true;
    }
    void changeCameraState() {
        camMode = (CAMERA_MODE)Util::wrap(camMode + 1, CAMERA_MODE::FIRST, CAMERA_MODE::FREE + 1);
        isFirstPerson = camMode == CAMERA_MODE::FIRST;
        isThirdPerson = camMode == CAMERA_MODE::THIRD;
        isFreeCam = camMode == CAMERA_MODE::FREE;
        printf("SWITCHED CAMERA MODE: %s\n", isFirstPerson ? "first person" : isThirdPerson ? "third person" : "free camera");
    }
}
