#include "player.h"

void Player::lookAt(vec3 p) {
    transform = Util::lookTowards(pos, p);
    // dir = normalize(p);
}

void Player::render() {
    // mesh->render(transform, 0);
    mesh->render(transform);
}

void Player::processMovement(Camera camera) {
    // todo: give camera front and up
    float t_cpos_y = pos.y;  // y-pos of camera before updates
    if (FORWARD) {
        if (CAN_FLY)
            pos += normalize(camera.front) * speed * SM::delta;
        else
            pos += normalize(vec3(camera.front.x, 0, camera.front.z)) * speed * SM::delta;
    }
    if (BACK) {
        if (CAN_FLY)
            pos -= normalize(camera.front) * speed * SM::delta;
        else
            pos -= normalize(vec3(camera.front.x, 0, camera.front.z)) * speed * SM::delta;
    }
    if (LEFT) {
        if (CAN_FLY)
            pos -= normalize(cross(camera.front, camera.up)) * speed * SM::delta;
        else {
            vec3 c = cross(camera.front, camera.up);
            pos -= normalize(vec3(c.x, 0, c.z)) * speed * SM::delta;
        }
    }
    if (RIGHT) {
        if (CAN_FLY)
            pos += normalize(cross(camera.front, camera.up)) * speed * SM::delta;
        else {
            vec3 c = cross(camera.front, camera.up);
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