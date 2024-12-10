#include "camera.h"
using namespace glm;

void Camera::processView() {
    // Euler angles
    pitch = std::clamp(pitch + (SM::mouseDY /* * SM::delta */ * -sensitivity), -89.5f, 89.5f);
    yaw = Util::wrap(yaw + (SM::mouseDX /* * SM::delta */ * sensitivity), 0.0f, 359.0f);
    front =
        normalize(vec3(cos(Util::d2r(yaw)) * cos(Util::d2r(pitch)),
                       sin(Util::d2r(pitch)),
                       sin(Util::d2r(yaw)) * cos(Util::d2r(pitch))));
    right = normalize(cross(front, Util::UP));
    up = normalize(cross(right, front));

    // Update view matrix
    view = SM::isFreeCam ? lookAt(pos, pos + front, up) : lookAt(followPos, followPos + front, up);

    // Follow checks
    if (SM::isFirstPerson) {
        targetVerticalDist = -0.25;
        targetHorizontalDist = fps_zm;
    } else {
        targetVerticalDist = targetDist * sin(Util::d2r(pitch));
        targetHorizontalDist = targetDist * cos(Util::d2r(pitch));
    }
}

void Camera::followTarget(vec3 tPos, vec3 tDir) {
    target = tPos;  // cache
    vec3 fDir = normalize(tDir);
    // rotation from z axis, calculated using rotation about y-axis
    // yaw needs to be negated because as the camera turns right, we want to move leftwards around it, and vice versa
    float angle = atan2f(fDir.z, fDir.x) - Util::d2r(yaw);
    float xDist = targetHorizontalDist * sin(angle);
    float zDist = targetHorizontalDist * cos(angle);
    pos = vec3(
        tPos.x + xDist,
        tPos.y - targetVerticalDist,
        tPos.z + zDist);
    followPos = Util::lerpV(followPos, pos, acceleration * SM::delta);
}

void Camera::followTarget(Player* player) {
    followTarget(vec3(player->transform[3]), player->dir);
}

void Camera::followTarget(Boid* b) {
    followTarget(b->pos, b->dir);
}

void Camera::processMovement() {
    float t_cpos_y = pos.y;  // y-pos of camera before updates
    if (FORWARD) {
        if (CAN_FLY)
            pos += normalize(front) * speed * SM::delta;
        else
            pos += normalize(vec3(front.x, 0, front.z)) * speed * SM::delta;
    }
    if (BACK) {
        if (CAN_FLY)
            pos -= normalize(front) * speed * SM::delta;
        else
            pos -= normalize(vec3(front.x, 0, front.z)) * speed * SM::delta;
    }
    if (LEFT) {
        if (CAN_FLY)
            pos -= normalize(cross(front, up)) * speed * SM::delta;
        else {
            vec3 c = cross(front, up);
            pos -= normalize(vec3(c.x, 0, c.z)) * speed * SM::delta;
        }
    }
    if (RIGHT) {
        if (CAN_FLY)
            pos += normalize(cross(front, up)) * speed * SM::delta;
        else {
            vec3 c = cross(front, up);
            pos += normalize(vec3(c.x, 0, c.z)) * speed * SM::delta;
        }
    }
    if (UP) {
        pos += vec3(0, speed * SM::delta, 0);
    };
    if (DOWN) {
        pos -= vec3(0, speed * SM::delta, 0);
    };

    speed = SPRINT ? sprintSpeed : baseSpeed;
    if (!CAN_FLY) pos.y = t_cpos_y;  // if can't fly, don't change y_pos
}

mat4 Camera::getViewMatrix() {
    return view;
}

mat4 Camera::getProjectionMatrix() {
    return perspectiveProjection;
}

void Camera::setPosition(vec3 p) {
    pos = p;
    followPos = p;
    processView();
}
