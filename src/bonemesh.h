#ifndef BONEMESH_H
#define BONEMESH_H
#pragma warning(disable : 26495)

#include <windows.h>
#include <mmsystem.h>

#include "mesh.h"
#include "shader.h"

#define B_AI_LOAD_FLAGS aiProcess_Triangulate

class BoneMesh : public Mesh {
   public:
    BoneMesh() { name = "NewBoneMesh" + std::to_string(SM::unnamedBoneMeshCount++); }
    BoneMesh(std::string nm) { name = nm; }
    BoneMesh(std::string nm, std::string mesh_path) {
        name = nm;
        if (!loadMesh(mesh_path, true)) std::cout << "\nfailed to load mesh \"" << nm.c_str() << "\" :(\n\n";
    }
    BoneMesh(std::string nm, std::string mesh_path, Shader* s) {
        name = nm;
        if (!loadMesh(mesh_path, true)) std::cout << "\nfailed to load mesh \"" << nm.c_str() << "\" :(\n\n";
        shader = s;
    }
    BoneMesh(std::string nm, std::string _mesh_path, int _atlasTileSize, int _atlasTilesUsed, bool load = true) {
        name = nm;
        mesh_path = _mesh_path;
        atlasTileSize = _atlasTileSize;
        atlasTilesUsed = _atlasTilesUsed;
        if (load)
            if (!loadMesh(mesh_path, true)) std::cout << "\nfailed to load mesh \"" << nm.c_str() << "\" :(\n\n";
    }
    BoneMesh(std::string nm, std::string _mesh_path, Shader* s, int _atlasTileSize, int _atlasTilesUsed, bool load = true) {
        name = nm;
        shader = s;
        mesh_path = _mesh_path;
        atlasTileSize = _atlasTileSize;
        atlasTilesUsed = _atlasTilesUsed;
        if (load)
            if (!loadMesh(mesh_path, true)) std::cout << "\nfailed to load mesh \"" << nm.c_str() << "\" :(\n\n";
    }

    ~BoneMesh();

    bool loadMesh(bool popBuffer) { return loadMesh(mesh_path, popBuffer); }    // load the mesh stored in the constructor
    bool loadMesh(std::string mesh_path) { return loadMesh(mesh_path, true); }  // load a mesh located at `mesh_path`
    bool loadMesh(std::string mesh_path, bool popBuffer);
    bool initScene(const aiScene*, std::string);
    void initSingleMesh(unsigned int, const aiMesh*);
    bool initMaterials(const aiScene*, std::string);
    bool loadDiffuseTexture(const aiMaterial* pMaterial, std::string dir, unsigned int index);
    bool loadSpecularTexture(const aiMaterial* pMaterial, std::string dir, unsigned int index);
    void populateBuffers();
    void render(unsigned int nInstances, const mat4* bone_trans_matrix, const float* depths);
    void render(unsigned int, const mat4*);
    void render(mat4);
    int getBoneID(const aiBone*);
    int createAnimationList(const aiNode* node, int childIdx, mat4 parent);
    std::vector<mat4> getBoneTransforms(float, float);
    std::vector<mat4> loadAnimation(float);
    const aiNodeAnim* findNodeAnim(const aiAnimation*, const std::string);
    std::vector<mat4> getUpdatedTransforms(float animSpeed);
    std::vector<mat4> getUpdatedTransforms(Shader* skinnedShader, float animSpeed);
    void update();                                        // update the mesh's animations
    void update(float animSpeed);                         // update the mesh's animations
    void update(Shader* skinnedShader);                   // update the mesh's animations using an external shader
    void update(Shader* skinnedShader, float animSpeed);  // update the mesh's animations using an external shader

#define SK_POSITION_LOC 0     // p_vbo
#define SK_NORMAL_LOC 1       // n_vbo
#define SK_TEXTURE_LOC 2      // t_vbo
#define SK_BONE_LOC 3         // bone vbo
#define SK_BONE_WEIGHT_LOC 4  // bone weight location
#define SK_INSTANCE_LOC 5     // instance location
#define SK_DEPTH_LOC 9        // instance location
};

#endif /* BONEMESH_H */
