#ifndef MESH_H
#define MESH_H
#pragma warning(disable : 26495)

#include <cassert>  // STL dynamic memory.
#include <cstdio>
#include <map>
#include <string>
#include <vector>  // STL dynamic memory.
#include <cmath>
#include <stddef.h>

#include <assimp/cimport.h>  // scene importer
#include <assimp/Importer.hpp>
#include <assimp/scene.h>        // collects data
#include <assimp/postprocess.h>  // various extra operations

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "util.h"
#include "sm.h"
#include "texture.h"

#define MAX_NUM_BONES_PER_VERTEX 4

class Mesh {
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

    struct Material {
        vec3 ambientColour = vec3(0.0f);
        vec3 diffuseColour = vec3(0.0f);
        vec3 specularColour = vec3(0.0f);
        Texture* diffTex = NULL;  // diffuse texture
        Texture* mtlsTex = NULL;  // metalness map
    };

    struct VertexBoneData {
        VertexBoneData() {
            for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; ++i) {
                boneIDs[i] = -1;
                weights[i] = 0.0f;
            }
        }

        unsigned int boneIDs[MAX_NUM_BONES_PER_VERTEX];
        float weights[MAX_NUM_BONES_PER_VERTEX];

        void addBoneData(unsigned int bID, float weight) {
            int cnt = 0;
            for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; ++i) {
                if (weights[i] < MIN_FLOAT_DIFF) {  // don't compare float values
                    boneIDs[i] = bID;
                    weights[i] = weight;
                    return;
                }
                cnt++;
            }
            if (cnt >= MAX_NUM_BONES_PER_VERTEX) assert(false && "too many bones per vertex (> 4)");
            assert(false && "no vertices affected by this bone");
        }
    };

    struct BoneInfo {
        aiMatrix4x4 offsetMatrix;
        aiMatrix4x4 lastTransformation;

        BoneInfo() {
            offsetMatrix = aiMatrix4x4();
            lastTransformation = aiMatrix4x4();
        }

        BoneInfo(const aiMatrix4x4& offset) {
            offsetMatrix = offset;
            lastTransformation = Util::GLMtoAI(mat4(0));
        }
    };

    // Create a new Mesh object without a mesh
    Mesh() { name = "NewMesh" + std::to_string(SM::unnamedMeshCount++); }

    // Create a new unnamed Mesh object
    Mesh(std::string nm) { name = nm; }

    virtual ~Mesh() = default;

    virtual bool loadMesh(std::string mesh_path) = 0;
    virtual void populateBuffers() = 0;

    std::string name;
    std::string mesh_path;
    mat4 mat;
    vec3 dir = vec3(1, 0, 0);
    unsigned int VAO = 0;          // mesh vao
    unsigned int p_VBO = 0;        // position vbo
    unsigned int n_VBO = 0;        // normal vbo
    unsigned int t_VBO = 0;        // texture vbo
    unsigned int d_VBO = 0;        // texture depth vbo
    unsigned int EBO = 0;          // index (element) vbo (ebo)
    unsigned int IBO = 0;          // instance vbo (ibo)
    int atlasTileSize = -1;        // size of a single tile in array texture (must be square)
    int atlasTilesUsed = -1;       // number of tiles in array texture (must be at last 1)
    bool usingAtlas = false;       // flag if the mesh is using an array texture
    bool populateBuffer = true;        // should this mesh's buffers be populated?

    std::vector<vec3> vertices;        // vertex positions
    std::vector<vec3> normals;          // vertex normals
    std::vector<vec2> texCoords;        // vertex texture coords
    std::vector<unsigned int> indices;  // vertex indices
    std::vector<MeshObject> meshes;     // submeshes in mesh
    std::vector<Material> materials;    // textures and colours in mesh
};

#endif /* MESH_H */
