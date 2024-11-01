#ifndef VARIANTMESH_H
#define VARIANTMESH_H

#pragma warning(disable : 26495)

#include <windows.h>
#include <mmsystem.h>

#include "mesh.h"
#include "shader.h"
#include "bonemesh.h"

#define MAX_NUM_BONES_PER_VERTEX 4

// Inherits the Mesh class. Can be used with instantiate any variant of meshes and textures using instancing, which allows for the retaining of 
// data read from the GPU via SSBOs.
class VariantMesh : public Mesh {
   public:
    struct DrawCommandBuffer {

    };

    VariantMesh() { name = "NewVariantMesh" + std::to_string(SM::unnamedBoneMeshCount++); }
    VariantMesh(std::string nm) { name = nm; }
    VariantMesh(std::string nm, std::string mesh_path) {
        name = nm;
        if (!loadMesh(mesh_path)) std::cout << "\n\nfailed to load mesh \"" << nm.c_str() << "\" :(\n";
    }
    VariantMesh(std::string nm, std::string mesh_path, Shader* s) {
        name = nm;
        if (!loadMesh(mesh_path)) std::cout << "\n\nfailed to load mesh \"" << nm.c_str() << "\" :(\n";
        shader = s;
    }

    ~VariantMesh();

    bool loadMesh(std::string mesh_path) { return loadMeshes({mesh_path}); }
    bool loadMeshes(std::vector<std::string> mesh_paths);
    bool initScene();
    void populateBuffers();
    void render(unsigned int, const mat4*);
    void render(mat4);
    void update();                       // update the mesh's animations
    void update(Shader* skinnedShader);  // update the mesh's animations using an external shader

#define VA_POSITION_LOC 0     // p_vbo
#define VA_NORMAL_LOC 1       // n_vbo
#define VA_TEXTURE_LOC 2      // t_vbo
#define VA_DEPTH_LOC 3        // d_vbo
#define VA_BONE_LOC 4         // bone vbo
#define VA_BONE_WEIGHT_LOC 5  // bone weight location
#define VA_INSTANCE_LOC 6     // instance location

    Shader* shader;
    std::vector<std::string> paths;
    std::vector<BoneMesh*> meshes;

    unsigned int ssbo;
};

#endif /* VARIANTMESH_H */
