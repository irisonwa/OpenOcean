#ifndef PLAYER_H
#define PLAYER_H

#include "util.h"
#include "mesh.h"
#include "bonemesh.h"
#include "camera.h"
#include "shader.h"

class Camera;

class Player {
   private:
    vec3 followPos; // follow position; used for interpolated movement
    float acceleration = 3; // lerp acceleration factor; to be multiplied by delta
   public:
    Player(std::string _name, std::string mesh_path, int mesh_atlas_size, int mesh_atlas_tiles_used, vec3 _pos, vec3 _dir) {
        name = _name;
        pos = _pos;
        followPos = _pos;
        dir = _dir;
        velocity = vec3(0);
        // mesh = new Mesh(name, mesh_path, mesh_atlas_size, mesh_atlas_tiles_used);
        mesh = new BoneMesh(name, mesh_path);
        transform = translate(transform, _pos);
    }
    Player(std::string _name, vec3 _pos, vec3 _dir) {
        name = _name;
        pos = _pos;
        followPos = _pos;
        dir = _dir;
        velocity = vec3(0);
        transform = translate(transform, _pos);
    }
    ~Player() {}

    void setMesh(std::string mesh_path, int _atlasTileSize, int _atlasTilesUsed);
    void processMovement(Camera camera); // process the player's movement using a camera POV
    void lookAt(vec3 p); // rotate the player to look at a point `p`
    void render(); // display the player on screen
    void setShader(Shader* shader); // set the shader for the player mesh

    // Mesh* mesh;
    BoneMesh* mesh;
    Shader* shader;

    std::string name;
    vec3 pos;
    vec3 dir;
    vec3 velocity;
    mat4 transform = mat4(1);
    float MAX_SPEED = 10;

    float pitch = 0.0;           // x-axis rotation (vertical)
    float yaw = 0.0;             // y-axis rotation (horizontal)
    float roll = 0.0;            // z-axis rotation
    float sensitivity = 0.15f;   // Camera sensitivity

    float baseSpeed = 5.0f;      // Walking speed (movement speed)
    float sprintSpeed = 100.0f;  // Sprinting speed (movement speed)
    float speed = baseSpeed;     // Camera speed (movement speed)

    // Keyboard movement
    bool FORWARD = false;  // z+
    bool BACK = false;     // z-
    bool LEFT = false;     // x-
    bool RIGHT = false;    // x+
    bool UP = false;       // y+
    bool DOWN = false;     // y-
    bool CAN_FLY = true;   // can the player fly (i.e., move on y-axis at will)?
    bool SPRINT = false;   // is the player sprinting?
};

#endif /* PLAYER_H */
