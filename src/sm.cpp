#include "sm.h"
#include "camera.h"  // fwd
#include "box.h"  // fwd

namespace SM {
int width = 1024;
int height = 600;

glm::vec4 bgColour = glm::vec4(0.2, 0.3, 0.5, 1);  // lightest colour of background. any distance fog should match this colour
glm::vec2 fogBounds = glm::vec2(25, 100);          // near and far bounds for fog
float updateDistance = 100;                        // distance at which to update boids
float seaLevel = 290;                              // y-level of ocean

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
CAMERA_MODE camMode = CAMERA_MODE::THIRD;
CAMERA_MODE lastCamMode = CAMERA_MODE::THIRD;
Camera *camera = new Camera(0.1f, 1000.0f, (float)SM::width / (float)SM::height);

Box *sceneBox = new Box(vec3(WORLD_BOUND_LOW * 2), vec3(WORLD_BOUND_HIGH * 2));

bool showNormal = false;
bool debug = false;
bool canBoidsAttack = true;

void updateDelta() {
    static DWORD last_time = 0;
    DWORD curr_time = timeGetTime();
    if (last_time == 0) last_time = curr_time;
    delta = (curr_time - last_time) * 0.001f;
    last_time = curr_time;
}

float getGlobalTime() {
    return ((float)(timeGetTime() - SM::startTime)) * 0.001f;
}

void updateMouse(int nx, int ny) {
    float xPos = width / 2.0;
    float yPos = height / 2.0;
    mouseDX = nx - xPos;
    mouseDY = ny - yPos;
}

void switchFirstAndThirdCam() {
    if (isFreeCam) return;
    camMode = (CAMERA_MODE)Util::wrap(camMode + 1, CAMERA_MODE::FIRST, CAMERA_MODE::THIRD + 1);
    isFirstPerson = camMode == CAMERA_MODE::FIRST;
    isThirdPerson = camMode == CAMERA_MODE::THIRD;
    printf("SWITCHED CAMERA MODE: %s\n", isFirstPerson ? "first person" : isThirdPerson ? "third person"
                                                                                        : "free camera");
}

void toggleFreeCam() {
    if (camMode == CAMERA_MODE::FREE) {
        // disable free cam
        camMode = lastCamMode;
        isFreeCam = false;
        isFirstPerson = camMode == CAMERA_MODE::FIRST;
        isThirdPerson = camMode == CAMERA_MODE::THIRD;
        printf("SWITCHED CAMERA MODE: %s\n", isFirstPerson ? "first person" : isThirdPerson ? "third person"
                                                                                            : "free camera");
    } else {
        // enable free cam
        lastCamMode = camMode;
        camMode = CAMERA_MODE::FREE;
        isFreeCam = true;
        isFirstPerson = false;
        isThirdPerson = false;
        printf("SWITCHED CAMERA MODE: free camera\n");
    }
}

void changeFlashlightState() {
    flashlightToggled = !flashlightToggled;
    // printf("FLASHLIGHT: %s\n", flashlightToggled ? "on" : "off");
}

void changeBoidAttackState() {
    canBoidsAttack = !canBoidsAttack;
    printf("BOID ATTACKING: %s\n", canBoidsAttack ? "true" : "false");
}

}  // namespace SM
