#include "player.h"

void Player::setMesh(std::string mesh_path, int _atlasTileSize, int _atlasTilesUsed) {
    mesh = new BoneMesh(name, mesh_path, _atlasTileSize, _atlasTilesUsed);
}

void Player::setShader(Shader* shader) {
    this->shader = shader;
}

void Player::lookAt(vec3 p) {
    dir = p;
    transform = Util::lookTowards(followPos, dir);
}

void Player::render() {
    mesh->update(shader, 10);
    mesh->render(transform);
}

void Player::processMovement() {
    float t_cpos_y = pos.y;  // y-pos of camera before updates
    if (FORWARD) {
        if (CAN_FLY)
            pos += normalize(SM::camera->front) * speed * SM::delta;
        else
            pos += normalize(vec3(SM::camera->front.x, 0, SM::camera->front.z)) * speed * SM::delta;
    }
    if (BACK) {
        if (CAN_FLY)
            pos -= normalize(SM::camera->front) * speed * SM::delta;
        else
            pos -= normalize(vec3(SM::camera->front.x, 0, SM::camera->front.z)) * speed * SM::delta;
    }
    if (LEFT) {
        if (CAN_FLY)
            pos -= normalize(cross(SM::camera->front, SM::camera->up)) * speed * SM::delta;
        else {
            vec3 c = cross(SM::camera->front, SM::camera->up);
            pos -= normalize(vec3(c.x, 0, c.z)) * speed * SM::delta;
        }
    }
    if (RIGHT) {
        if (CAN_FLY)
            pos += normalize(cross(SM::camera->front, SM::camera->up)) * speed * SM::delta;
        else {
            vec3 c = cross(SM::camera->front, SM::camera->up);
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

    pos = vec3(
        Util::clamp(pos.x, WORLD_BOUND_LOW, WORLD_BOUND_HIGH + 100),
        Util::clamp(pos.y, WORLD_BOUND_LOW, WORLD_BOUND_HIGH + 100),
        Util::clamp(pos.z, WORLD_BOUND_LOW, WORLD_BOUND_HIGH + 100));
    followPos = Util::lerpV(followPos, pos, SM::delta * acceleration);
}
