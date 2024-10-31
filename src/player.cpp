#include "player.h"

void Player::lookAt(vec3 p) {
    transform = Util::lookTowards(followPos, p);
    // dir = p;
}

void Player::render() {
    mesh->update(shader);
    mesh->render(transform);
}

void Player::setShader(Shader* shader) {
    this->shader = shader;
}

void Player::processMovement(Camera camera) {
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

    followPos = Util::lerpV(followPos, pos, SM::delta * acceleration);

    // Keep player within world bounds
    float tf = 0.2;
    if (followPos.x < SM::WORLD_BOUND_LOW)
        followPos.x += tf;
    if (followPos.x > SM::WORLD_BOUND_HIGH)
        followPos.x -= tf;
    if (followPos.y < SM::WORLD_BOUND_LOW)
        followPos.y += tf;
    if (followPos.y > SM::WORLD_BOUND_HIGH)
        followPos.y -= tf;
    if (followPos.z < SM::WORLD_BOUND_LOW)
        followPos.z += tf;
    if (followPos.z > SM::WORLD_BOUND_HIGH)
        followPos.z -= tf;

}
