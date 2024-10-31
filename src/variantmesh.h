#ifndef VARIANTMESH_H
#define VARIANTMESH_H

#pragma warning(disable : 26495)

#include <windows.h>
#include <mmsystem.h>

#include "mesh.h"
#include "shader.h"
#include "bonemesh.h"

#define MAX_NUM_BONES_PER_VERTEX 4

class VariantMesh : public Mesh {
   public:
    struct MeshObject {
        MeshObject() {
            n_Indices = 0;
            baseVertex = 0;
            baseIndex = 0;
            materialIndex = 0;
        }
        unsigned int n_Indices;
        unsigned int baseVertex;
        unsigned int baseIndex;
        unsigned int materialIndex;
    };

    struct VertexBoneData {
        VertexBoneData() {
            for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; ++i) {
                BoneIDs[i] = -1;
                Weights[i] = 0.0f;
            }
        }

        unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX];
        float Weights[MAX_NUM_BONES_PER_VERTEX];

        void addBoneData(unsigned int BoneID, float Weight) {
            for (unsigned int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++) {
                if (Weights[i] < 0.00001f) {
                    BoneIDs[i] = BoneID;
                    Weights[i] = Weight;
                    return;
                }
            }
            assert(false && "too many bones per vertex (> 4)");
        }
    };

    struct BoneInfo {
        aiMatrix4x4 offsetMatrix;
        aiMatrix4x4 lastTransformation;

        BoneInfo(const aiMatrix4x4& offset) {
            offsetMatrix = offset;
            lastTransformation = Util::GLMtoAI(mat4(0));
        }
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

    aiMatrix4x4 globalInverseTrans;  // inverse matrix
    unsigned int BBO;                // bone vbo

    std::vector<VertexBoneData> m_Bones;
    std::vector<BoneInfo*> m_BoneInfo;

    std::map<std::string, unsigned int> boneToIndexMap;

    const aiScene* scene;
    Assimp::Importer importer;

    Shader* shader;

    std::vector<std::string> paths;
    std::vector<BoneMesh*> meshes;
};

#endif /* VARIANTMESH_H */
