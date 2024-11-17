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
#include "shader.h"

#define MAX_NUM_BONES_PER_VERTEX 4
#define MAX_JOINTS_PER_BONE 16  // maximum number of children a bone can have
#define MAX_KEYFRAMES 32        // maximum number of keyframes an animation can have

class Mesh {
   public:
    // Information about a mesh object. In Blender, this would be each mesh in each collection in the scene. Used for instancing (`glDrawElementsInstancedBaseVertex`).
    // For this project, each model is assumed to have one mesh.
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

    // Struct containing a diffuse texture and metalness texture.
    struct Material {
        Texture* diffTex = NULL;  // diffuse texture
        Texture* mtlsTex = NULL;  // metalness map
    };

    // Information about the bones and weights considered for a vertex. Allows at most 4 such influences.
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
            if (cnt >= MAX_NUM_BONES_PER_VERTEX - 1) assert(false && "too many bones per vertex (> 4)");
            assert(false && "no vertices affected by this bone");
        }
    };

    // Information about a bone. Includes offset/inverse-bind pose matrix and current transformation of the bone in local space.
    struct BoneInfo {
        mat4 offsetMatrix;
        mat4 currentTransformation;
        int ID;
        int children[MAX_JOINTS_PER_BONE];
        vec2 pdding = vec2(0);
        int pdding2 = 0;

        BoneInfo (int id) {
            ID = id;
            for (int i = 0; i < MAX_JOINTS_PER_BONE; ++i) {
                children[i] = -1;
            }
            offsetMatrix = mat4(1);
            currentTransformation = mat4(1);
        }

        BoneInfo(const mat4& offset) {
            offsetMatrix = offset;
            currentTransformation = mat4(0);
        }
    };

    // Information about a bone's animation.
    // Includes the length of the animation, the bone's transformation matrix associated with the mesh, and the offset of the matrices for use in an Animation buffer.
    struct Animation {
        int boneIndex;                  // index of bone this animation applies to
        float animationLength;                   // length of the animation in ticks
        mat4 relTransformation;                  // transformation relative to parent (aiNode->mTransformation)
        mat4 globalInvTransform;                 // global inverse transform of root node of mesh's skeleton
        vec4 positionKeys[MAX_KEYFRAMES];        // changes in position (first 3) and time of keys (last float)
        vec4 scalingKeys[MAX_KEYFRAMES];         // changes in scaling (first 3) and time of keys (last float)
        quat rotationKeys[MAX_KEYFRAMES];        // changes in rotation
        float rotationKeysTimes[MAX_KEYFRAMES];  // times of rotation keys
        vec2 pdding = vec2(0);

        Animation() {
            for (int i = 0; i < MAX_KEYFRAMES; ++i) {
                positionKeys[i] = vec4(-1);
                scalingKeys[i] = vec4(-1);
                rotationKeys[i] = quat(-1, -1, -1, -1);
                rotationKeysTimes[i] = -1;
            }
        }
        vec3 interpolatePosition(float atime) {
            if (positionKeys[1].x == -1) {
                return positionKeys[0];
            }

            // get position index
            unsigned int positionIndex = 0;
            for (unsigned int i = 0; i < MAX_KEYFRAMES - 1; i++) {
                float t = positionKeys[i + 1].w;  // time of key
                if (atime < t) {
                    positionIndex = i;
                    break;
                }
            }
            assert(positionIndex + 1 < MAX_KEYFRAMES);

            float deltaTime = positionKeys[positionIndex + 1].w - positionKeys[positionIndex].w;  // delta between two adjacent Position keyfraes
            float factor = (atime - positionKeys[positionIndex].w) / deltaTime;                   // intermediate position factor (mapped to 0-1)

            const vec3 start = vec3(positionKeys[positionIndex]);    // start position value
            const vec3 end = vec3(positionKeys[positionIndex + 1]);  // end position value
            return start + factor * (end - start);                   // interpolated position
        }

        vec3 interpolateScale(float atime) {
            if (scalingKeys[1].x == -1) {
                return scalingKeys[0];
            }

            // get scale index
            unsigned int scalingIndex = 0;
            for (unsigned int i = 0; i < MAX_KEYFRAMES - 1; i++) {
                float t = scalingKeys[i + 1].w;  // time of key
                if (atime < t) {
                    scalingIndex = i;
                    break;
                }
            }
            assert(scalingIndex + 1 < MAX_KEYFRAMES);

            float deltaTime = scalingKeys[scalingIndex + 1].w - scalingKeys[scalingIndex].w;  // delta between two adjacent scaling keyfraes
            float factor = (atime - scalingKeys[scalingIndex].w) / deltaTime;                 // intermediate scaling factor (mapped to 0-1)

            const vec3 start = vec3(scalingKeys[scalingIndex]);    // start scaling value
            const vec3 end = vec3(scalingKeys[scalingIndex + 1]);  // end scaling value
            return start + factor * (end - start);                 // interpolated scaling
        }

        quat interpolateRotation(float atime) {
            if (rotationKeys[1].x == -1) {
                return rotationKeys[0];
            }

            // get scale index
            unsigned int rotationIndex = 0;
            for (unsigned int i = 0; i < MAX_KEYFRAMES - 1; i++) {
                float t = rotationKeysTimes[i + 1];
                if (atime < t) {
                    rotationIndex = i;
                    break;
                }
            }
            assert(rotationIndex + 1 < MAX_KEYFRAMES);

            float deltaTime = rotationKeysTimes[rotationIndex + 1] - rotationKeysTimes[rotationIndex];  // delta between two adjacent rotation keyfraes
            float factor = (atime - rotationKeysTimes[rotationIndex]) / deltaTime;                      // intermediate rotation factor (mapped to 0-1)

            const quat start = rotationKeys[rotationIndex];    // start rotation value
            const quat end = rotationKeys[rotationIndex + 1];  // end rotation value
            return normalize(slerp(start, end, factor));       // interpolated rotation
        }
    };

    // Create a new Mesh object without a mesh
    Mesh() { name = "NewMesh" + std::to_string(SM::unnamedMeshCount++); }

    // Create a new unnamed Mesh object
    Mesh(std::string nm) { name = nm; }

    virtual ~Mesh() = default;

    virtual bool loadMesh(std::string mesh_path) = 0;
    virtual bool loadMesh(std::string mesh_path, bool popBuffers) = 0;
    virtual void populateBuffers() = 0;
    virtual std::vector<mat4> getUpdatedTransforms(Shader* skinnedShader, float animSpeed) = 0;
    virtual std::vector<mat4> getUpdatedTransforms(float animSpeed) = 0;
    virtual void update() = 0;
    virtual void update(float speed) = 0;
    virtual void update(Shader* shader) = 0;
    virtual void update(Shader* shader, float speed) = 0;

    std::string name;
    std::string mesh_path;
    mat4 mat;
    vec3 dir = vec3(1, 0, 0);
    Shader* shader;                                      // shader used to render mesh
    unsigned int VAO = 0;                                // mesh vao
    unsigned int p_VBO = 0;                              // position vbo
    unsigned int n_VBO = 0;                              // normal vbo
    unsigned int t_VBO = 0;                              // texture vbo
    unsigned int d_VBO = 0;                              // texture depth vbo
    unsigned int EBO = 0;                                // index (element) vbo (ebo)
    unsigned int IBO = 0;                                // instance vbo (ibo)
    unsigned int BBO = 0;                                // bone ids and weights vbo
    unsigned int ABBO;                                   // animated bone transform ssbo
    unsigned int BIBO;                                   // bone info ssbo
    int atlasTileSize = -1;                              // size of a single tile in array texture (must be square)
    int atlasTilesUsed = -1;                             // number of tiles in array texture (must be at last 1)
    bool usingAtlas = false;                             // flag if the mesh is using an array texture
    bool populateBuffer = true;                          // should this mesh's buffers be populated?
    std::vector<vec3> vertices;                          // vertex positions
    std::vector<vec3> normals;                           // vertex normals
    std::vector<vec2> texCoords;                         // vertex texture coords
    std::vector<unsigned int> indices;                   // vertex indices
    std::vector<MeshObject> meshes;                      // submeshes in mesh
    std::vector<Material> materials;                     // textures and colours in mesh
    std::vector<VertexBoneData> vBones;                  // vertex-bone influences
    std::vector<BoneInfo> boneInfos;                     // bones
    std::vector<Animation> animations;                   // animations for each bone
    aiMatrix4x4 globalInverseTrans;                      // inverse-bind pose matrix
    std::map<std::string, unsigned int> boneToIndexMap;  // mapping bone name to numerical index
    const aiScene* scene;                                // mesh scene loaded from assimp
    Assimp::Importer importer;                           // must be kept to import scene
};

#endif /* MESH_H */
