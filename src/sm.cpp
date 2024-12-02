#include "sm.h"
#include "camera.h" // fwd

namespace SM {
int width = 1024;
int height = 600;

glm::vec4 bgColour = glm::vec4(0.2, 0.3, 0.5, 1);  // lightest colour of background. any distance fog should match this colour
glm::vec2 fogBounds = glm::vec2(25, 200);          // near and far bounds for fog
float seaLevel = 200;                              // y-level of ocean

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
Camera *camera = new Camera(0.1f, 1000.0f, (float)SM::width / (float)SM::height);
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

float getGlobalTime() {
    return ((float)(timeGetTime() - SM::startTime)) * 0.001f;
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
    printf("SWITCHED CAMERA MODE: %s\n", isFirstPerson ? "first person" : isThirdPerson ? "third person"
                                                                                        : "free camera");
}
}  // namespace SM
