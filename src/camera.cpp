#include "camera.h"
using namespace glm;
using namespace SM;

void Camera::processView(int x, int y) {
    float xPos = SM::width / 2.0;
    float yPos = SM::height / 2.0;
    pitch = std::clamp(pitch + ((y - yPos) * -sensitivity), -89.0f, 89.0f);
    yaw = Util::wrap(yaw + ((x - xPos) * sensitivity), 0.0f, 359.0f);
    glutWarpPointer(xPos, yPos);

    front =
        normalize(vec3(cos(Util::deg2Rad(yaw)) * cos(Util::deg2Rad(pitch)),
                       sin(Util::deg2Rad(pitch)),
                       sin(Util::deg2Rad(yaw)) * cos(Util::deg2Rad(pitch))));

    right = normalize(cross(front, wUP));
    up = normalize(cross(right, front));
}

void Camera::processMovement() {
    float t_cpos_y = pos.y;  // y-pos of camera before updates
    if (FORWARD) {
        if (CAN_FLY)
            pos += normalize(front) * speed * delta;
        else
            pos += normalize(vec3(front.x, 0, front.z)) * speed * delta;
    }
    if (BACK) {
        if (CAN_FLY)
            pos -= normalize(front) * speed * delta;
        else
            pos -= normalize(vec3(front.x, 0, front.z)) * speed * delta;
    }
    if (LEFT) {
        if (CAN_FLY)
            pos -= normalize(cross(front, up)) * speed * delta;
        else {
            vec3 c = cross(front, up);
            pos -= normalize(vec3(c.x, 0, c.z)) * speed * delta;
        }
    }
    if (RIGHT) {
        if (CAN_FLY)
            pos += normalize(cross(front, up)) * speed * delta;
        else {
            vec3 c = cross(front, up);
            pos += normalize(vec3(c.x, 0, c.z)) * speed * delta;
        }
    }
    if (UP) {
        pos += vec3(0, speed * SM::delta, 0);
    };
    if (DOWN) {
        pos -= vec3(0, speed * SM::delta, 0);
    };

    speed = SPRINT ? sprintSpeed : baseSpeed;

    /* // if sprinting (and moving), change the FOV on start and when stopped.
    int lerp_speed = 350;
    FOV = (SPRINT && ((CAN_FLY && UP) || (CAN_FLY && DOWN) || LEFT || RIGHT || FORWARD || BACK)) ? 
        std::clamp(Util::lerp(FOV, max_FOV, SM::delta * lerp_speed), base_FOV, max_FOV) : // zoom out
        std::clamp(Util::lerp(FOV, base_FOV, SM::delta * -lerp_speed * 2), base_FOV, max_FOV); // zoom in */

    if (!CAN_FLY) pos.y = t_cpos_y;  // if can't fly, don't change y_pos
}

mat4 Camera::getViewMatrix() { return lookAt(pos, pos + front, up); }
