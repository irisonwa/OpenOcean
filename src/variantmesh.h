#ifndef VARIANTMESH_H
#define VARIANTMESH_H

#pragma warning(disable : 26495)

#include <windows.h>
#include <mmsystem.h>

#include "mesh.h"
#include "staticmesh.h"
#include "bonemesh.h"
#include "shader.h"

enum VariantType {
    STATIC,
    SKINNED
};

// Inherits the Mesh class. Can be used with instantiate any variant of meshes and textures using instancing, which allows for the retaining of
// data read from the GPU via SSBOs.
class VariantMesh : public Mesh {
   public:
    struct IndirectDrawCommand {
        unsigned int indexCount;
        unsigned int instanceCount;
        unsigned int baseIndex;
        unsigned int baseVertex;
        unsigned int baseInstance;
    };

    struct VariantInfo {
        VariantInfo(std::string parentName_,
                    std::string path_,
                    VariantType type_,
                    unsigned int instanceCount_,
                    unsigned int textureAtlasSize_,
                    unsigned int textureAtlasTileCount_,
                    std::vector<unsigned int> depths_) {
            parentName = parentName_;
            path = path_;
            instanceCount = instanceCount_;
            textureAtlasSize = textureAtlasSize_;
            textureAtlasTileCount = textureAtlasTileCount_;
            depths = depths_;
            type = type_;
        }
        // Load the mesh stored in this variant without populating its buffers
        bool loadMesh() {
            using enum VariantType;
            if (type == STATIC) mesh = new StaticMesh(parentName + "_" + MODEL_NO_DIR(path), path, textureAtlasSize, textureAtlasTileCount, false);
            else if (type == SKINNED) mesh = new BoneMesh(parentName + "_" + MODEL_NO_DIR(path), path, textureAtlasSize, textureAtlasTileCount, false);
            return mesh->loadMesh(path, false);
        }
        std::string parentName;
        std::string path;
        unsigned int instanceCount;
        unsigned int textureAtlasSize;
        unsigned int textureAtlasTileCount;
        std::vector<unsigned int> depths;
        Mesh* mesh;
        VariantType type;
    };

    VariantMesh() { name = "NewVariantMesh" + std::to_string(SM::unnamedVariantMeshCount++); }
    VariantMesh(std::string nm) { name = nm; }
    /*
        Create a new VariantMesh. `nm` is the name of the mesh. `s` is a pointer to the shader to use.
        `_variants` is a list of tuples, each of which represents one mesh. each tuple has:
        - `string`: path to a mesh
        - `unsigned int`: the number of instances of this mesh to draw
        - `unsigned int`: the square size of the array texture of size `size x size`. if -1, will use texture of size `img_width x img_width`
        - `unsigned int`: the number of tiles in the texture, vertically. must be at least 1. if -1, will be set to 1.
        - `vector<unsigned int>`: a list of depths to use in the mesh. must have a size equal to the number of instances (first unsigned int)
     */
    VariantMesh(std::string nm, Shader* s, VariantType type_, std::vector<std::tuple<std::string, unsigned int, unsigned int, unsigned int, std::vector<unsigned int>>> variants_) {
        name = nm;
        shader = s;
        type = type_;
        animShader = new Shader("anim shader", PROJDIR "Shaders/anim.comp");
        for (const auto& [path_, instanceCount_, textureAtlasSize_, textureAtlasTileCount_, depths_] : variants_) {
            VariantInfo* vi = new VariantInfo(name, path_, type, instanceCount_, textureAtlasSize_, textureAtlasTileCount_, depths_);
            variants.push_back(vi);
        }
        if (!loadMeshes()) std::cout << "\n\nfailed to load variant mesh \"" << nm.c_str() << "\" :(\n";
    }

    ~VariantMesh();

    bool loadMesh(std::string) { return true; }        // should probably remove this from base class? idk
    bool loadMesh(std::string, bool) { return true; }  // should probably remove this from base class? idk
    bool loadMeshes() { return loadMeshes(variants); }
    bool loadMeshes(std::vector<VariantInfo*> variantInfos);
    bool initScene();
    void loadMaterials();
    void unloadMaterials();
    void generateCommands();
    void populateBuffers();
    void render(const mat4*);
    void render(mat4);
    void render();
    std::vector<mat4> getUpdatedTransforms(Shader* skinnedShader, float animSpeed) { return {}; }  // unused
    std::vector<mat4> getUpdatedTransforms(float animSpeed) { return {}; }                         // unused
    void update() {}                                                                   // unused
    void update(float speed) {}                                                        // unused
    void update(Shader* shader) {}                                                     // unused
    void update(Shader* shader, float speed) {}                                        // unused

#define VA_POSITION_LOC 0     // p_vbo
#define VA_NORMAL_LOC 1       // n_vbo
#define VA_TEXTURE_LOC 2      // t_vbo
#define VA_BONE_LOC 3         // bone id vbo
#define VA_BONE_WEIGHT_LOC 4  // bone weight vbo
#define VA_INSTANCE_LOC 5     // instance vbo
#define VA_DEPTH_LOC 9        // texture depth vbo

    unsigned int ABBO;                        // animated bone transform ssbo
    unsigned int BIBO;                        // bone info ssbo
    unsigned int BOBO;                        // bone offset ssbo
    unsigned int commandBuffer;               // draw command buffer object (compute shader)
    int totalInstanceCount = 0;               // number of instances across all variants
    std::vector<float> depths;                // texture depths for each variant
    std::vector<int> boneTransformOffsets;    // number of bones in each variant
    std::vector<mat4> globalInverseMatrices;  // global inverse matrix for each variant
    std::vector<std::string> paths;           // paths of variant mesh files
    std::vector<VariantInfo*> variants;       // variant meshes held in this object
    Shader* animShader;
    VariantType type;
};

#endif /* VARIANTMESH_H */
