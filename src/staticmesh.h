#ifndef STATICMESH_H
#define STATICMESH_H

#pragma warning(disable : 26495)

#include "mesh.h"

#define AI_LOAD_FLAGS aiProcess_Triangulate | aiProcess_PreTransformVertices

class StaticMesh : public Mesh {
   public:
    // Create a new Mesh object without a mesh
    StaticMesh() { name = "NewMesh" + std::to_string(SM::unnamedMeshCount++); }

    // Create a new unnamed Mesh object
    StaticMesh(std::string mesh_path) {
        name = "NewMesh" + std::to_string(SM::unnamedMeshCount++);
        if (!loadMesh(mesh_path)) std::cout << "\n\nfailed to load mesh \"" << mesh_path.c_str() << "\" :(\n";
    }

    // Create a new Mesh object and give it a name.
    StaticMesh(std::string nm, std::string mesh_path) {
        name = nm;
        if (!loadMesh(mesh_path)) std::cout << "\n\nfailed to load mesh \"" << nm.c_str() << "\" :(\n";
    }

    // Create a new Mesh object from a atlas texture.
    // The width of the texture must be `_atlasTileSize` and the length must be at least `_atlasTileSize` * `_atlasTilesUsed`
    // If `isVerticalTexture` is `true`, the texture is assumed to be one tile wide and `_atlasTilesUsed` tiles long.
    // If `isVerticalTexture` is `false`, it is assumed the texture only uses tiles of size `_atlasTileSize`.
    StaticMesh(std::string nm, std::string mesh_path, int _atlasTileSize, int _atlasTilesUsed, bool load = true) {
        name = nm;
        usingAtlas = true;
        atlasTileSize = _atlasTileSize;
        atlasTilesUsed = _atlasTilesUsed;
        if (load) if (!loadMesh(mesh_path)) std::cout << "\n\nfailed to load mesh \"" << nm.c_str() << "\" :(\n";
    }

    ~StaticMesh();

    bool loadMesh(std::string mesh_path) { return loadMesh(mesh_path, true); }
    bool loadMesh(std::string mesh_path, bool popBuffer);
    bool initScene(const aiScene*, std::string);
    void initSingleMesh(const aiMesh*);
    bool initMaterials(const aiScene*, std::string);
    void populateBuffers();
    void render(unsigned int, const mat4*);                // render an array of meshes using instancing
    void render(unsigned int, const mat4*, const float*);  // render an array of meshes using instancing and atlas depths
    void render(mat4);                                     // render a single mesh
    void render(mat4, float);                              // single atlas depth

#define ST_POSITION_LOC 0  // p_vbo
#define ST_NORMAL_LOC 1    // n_vbo
#define ST_TEXTURE_LOC 2   // t_vbo
#define ST_INSTANCE_LOC 3
#define ST_DEPTH_LOC 7  // texture depth
};

#endif /* STATICMESH_H */
