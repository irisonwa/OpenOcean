#ifndef BONEMESH_H
#define BONEMESH_H
#pragma once
#pragma warning(disable : 26495)

#include <assert.h>
#include <windows.h>
#include <mmsystem.h>
#include <map>
#include <string>
#include <vector>

#include <assimp/cimport.h>  // scene importer
#include <assimp/Importer.hpp>
#include <assimp/scene.h>        // collects data
#include <assimp/postprocess.h>  // various extra operations

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "util.h"
#include "texture.h"
#include "shader.h"
#include "sm.h"

#define MAX_NUM_BONES_PER_VERTEX 4
#define B_AI_LOAD_FLAGS aiProcess_Triangulate | aiProcess_FixInfacingNormals 

class BoneMesh {
   public:
    struct VertexBoneData {
        VertexBoneData() {}

        unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = {0};
        float Weights[MAX_NUM_BONES_PER_VERTEX] = {0.0f};

        void addBoneData(unsigned int BoneID, float Weight) {
            for (unsigned int i = 0; i < ARRAY_SIZE(BoneIDs); i++) {
                if (Weights[i] == 0.0) {
                    BoneIDs[i] = BoneID;
                    Weights[i] = Weight;
                    // printf("bone %d weight %f index %i\n", BoneID, Weight, i);
                    return;
                }
            }

            // should never get here - more bones than we have space for
            assert(0);
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

    struct MeshObject {
        MeshObject() {
            n_Indices = 0;
            baseVertex = 0;
            baseIndex = 0;
            materialIndex = 0;
        }
        unsigned int n_Indices;
        int baseVertex;
        unsigned int baseIndex;
        unsigned int materialIndex;
    };

    struct Material {
        vec3 ambientColour = vec3(0.0f);
        vec3 diffuseColour = vec3(0.0f);
        vec3 specularColour = vec3(0.0f);
        Texture* diffTex = NULL;
        Texture* specExp = NULL;
    };

    BoneMesh() { name = "NewBoneMesh" + std::to_string(SM::unnamedBoneMeshCount++); }
    BoneMesh(std::string nm) { name = nm; }
    BoneMesh(std::string nm, std::string mesh_path) { 
        name = nm;
        if (!loadMesh(mesh_path)) std::cout << "\n\nfailed to load mesh \"" << nm.c_str() << "\" :(\n";
    }
    bool loadMesh(std::string mesh_path);
    bool initScene(const aiScene*, std::string);
    void initSingleMesh(unsigned int, const aiMesh*);
    bool initMaterials(const aiScene*, std::string);
    void loadDiffuseTexture(const aiMaterial* pMaterial, std::string dir, unsigned int index);
    void loadSpecularTexture(const aiMaterial* pMaterial, std::string dir, unsigned int index);
    void loadColours(const aiMaterial*, int);
    void populateBuffers();
    void render(unsigned int, const mat4*);
    void render(mat4);
    const Material& getMaterial();
    void loadMeshBones(unsigned int, const aiMesh*);
    void loadSingleBone(unsigned int, const aiBone*);
    int getBoneID(const aiBone*);
    void getBoneTransforms(float, std::vector<aiMatrix4x4>&);
    void readNodeHierarchy(float, const aiNode*, const aiMatrix4x4&);
    const aiNodeAnim* findNodeAnim(const aiAnimation*, const std::string);
    void calcInterpolatedTranslation(aiVector3D&, float, const aiNodeAnim*);
    void calcInterpolatedScale(aiVector3D&, float, const aiNodeAnim*);
    void calcInterpolatedRotation(aiQuaternion&, float, const aiNodeAnim*);
    void update(Shader* skinnedShader);  // update the mesh's animations

#define BPOSITION_LOC 0     // p_vbo
#define BNORMAL_LOC 1       // n_vbo
#define BTEXTURE_LOC 2      // t_vbo
#define BBONE_LOC 3         // bone vbo
#define BBONE_WEIGHT_LOC 4  // bone weight location
#define BINSTANCE_LOC 5     // instance location

    std::string name;
    mat4 mat;
    aiMatrix4x4 globalInverseTrans;  // inverse matrix
    unsigned int VAO;                // mesh vao
    unsigned int p_VBO;              // position vbo
    unsigned int n_VBO;              // normal vbo
    unsigned int t_VBO;              // texture vbo
    unsigned int EBO;                // index (element) vbo (ebo)
    unsigned int IBO;                // instance vbo (ibo)
    unsigned int BBO;                // bone vbo

    std::vector<vec3> m_Positions;
    std::vector<vec3> m_Normals;
    std::vector<vec2> m_TexCoords;
    std::vector<unsigned int> m_Indices;

    std::vector<MeshObject> m_Meshes;
    std::vector<Material> m_Materials;
    std::vector<VertexBoneData> m_Bones;
    std::vector<BoneInfo*> m_BoneInfo;

    std::map<std::string, unsigned int> boneToIndexMap;

    const aiScene* scene;
    Assimp::Importer importer;
};

#endif /* BONEMESH_H */
