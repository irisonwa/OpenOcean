#ifndef VARIANTMESH_H
#define VARIANTMESH_H

#pragma warning(disable : 26495)

#include <windows.h>
#include <mmsystem.h>

#include "mesh.h"
#include "staticmesh.h"
#include "bonemesh.h"
#include "shader.h"

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
        VariantInfo(std::string _parentName,
                    std::string _path,
                    unsigned int _instanceCount,
                    unsigned int _textureAtlasSize,
                    unsigned int _textureAtlasTileCount,
                    std::vector<unsigned int> _depths) {
            parentName = _parentName;
            path = _path;
            instanceCount = _instanceCount;
            textureAtlasSize = _textureAtlasSize;
            textureAtlasTileCount = _textureAtlasTileCount;
            depths = _depths;
        }
        // Load the mesh stored in this variant without populating its buffers
        bool loadMesh() {
            // mesh = new BoneMesh(parentName + "_" + MODEL_NO_DIR(path), path, textureAtlasSize, textureAtlasTileCount, false);
            mesh = new StaticMesh(parentName + "_" + MODEL_NO_DIR(path), path, textureAtlasSize, textureAtlasTileCount, false);
            return mesh->loadMesh(path, false);
        }
        std::string parentName;
        std::string path;
        unsigned int instanceCount;
        unsigned int textureAtlasSize;
        unsigned int textureAtlasTileCount;
        std::vector<unsigned int> depths;
        // BoneMesh* mesh;
        StaticMesh* mesh;
    };

    VariantMesh() { name = "NewVariantMesh" + std::to_string(SM::unnamedVariantMeshCount++); }
    VariantMesh(std::string nm) { name = nm; }
    /*
     * Create a new VariantMesh. `nm` is the name of the mesh. `s` is a pointer to the shader to use.
     * `_variants` is a list of tuples, each of which represents one mesh. each tuple has:
     * - `string`: path to a mesh
     * - `unsigned int`: the number of instances of this mesh to draw
     * - `unsigned int`: the square size of the array texture the mesh will use which must be a square texture of size `size x size`. if -1, will use the entire texture available
     * - `unsigned int`: the number of tiles in the texture, vertically. must be at least 1. if -1, will be set to 1.
     * - `vector<unsigned int>`: a list of depths to use in the mesh. must have a size equal to the number of instances (first int)
     */
    VariantMesh(std::string nm, Shader* s, std::vector<std::tuple<std::string, unsigned int, unsigned int, unsigned int, std::vector<unsigned int>>> _variants) {
        name = nm;
        shader = s;
        for (const auto& [_path, _instanceCount, _textureAtlasSize, _textureAtlasTileCount, _depths] : _variants) {
            VariantInfo* vi = new VariantInfo(name, _path, _instanceCount, _textureAtlasSize, _textureAtlasTileCount, _depths);
            variants.push_back(vi);
        }
        if (!loadMeshes()) std::cout << "\n\nfailed to load variant mesh \"" << nm.c_str() << "\" :(\n";
    }

    ~VariantMesh();

    bool loadMesh(std::string) { return true; }  // should probably remove this from base class? idk
    bool loadMeshes() { return loadMeshes(variants); }
    bool loadMeshes(std::vector<VariantInfo*> variantInfos);
    bool initScene();
    void loadMaterials();
    void unloadMaterials();
    void generateCommands();
    void populateBuffers();
    void render(const mat4*);
    void render(mat4);
    void update();                       // update the mesh's animations
    void update(Shader* skinnedShader);  // update the mesh's animations using an external shader

#define VA_POSITION_LOC 0     // p_vbo
#define VA_NORMAL_LOC 1       // n_vbo
#define VA_TEXTURE_LOC 2      // t_vbo
#define VA_BONE_LOC 3         // bone vbo
#define VA_BONE_WEIGHT_LOC 4  // bone weight location
#define VA_BONE_TRANS_LOC 5   // bone transform location
#define VA_INSTANCE_LOC 9     // instance location
#define VA_DEPTH_LOC 13       // texture depth vbo

    Shader* shader;

    unsigned int commandBuffer;  // draw command buffer object (compute shader)
    unsigned int BBO;            // bone vertex vbo
    unsigned int BTBO;           // bone transform vbo

    std::vector<VertexBoneData> m_Bones;
    std::vector<BoneInfo*> m_BoneInfo;
    std::vector<std::string> paths;
    std::vector<VariantInfo*> variants;
    std::vector<float> depths;
    int totalInstanceCount = 0;
};

#endif /* VARIANTMESH_H */
