#ifndef MESH_H
#define MESH_H
#pragma warning(disable : 26495)

#include <string>
#include <map>
#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <vector>    // STL dynamic memory.
#include <assert.h>  // STL dynamic memory.

#include <assimp/cimport.h>  // scene importer
#include <assimp/Importer.hpp>
#include <assimp/scene.h>        // collects data
#include <assimp/postprocess.h>  // various extra operations

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "util.h"
#include "sm.h"
#include "texture.h"

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
        Texture* diffTex = NULL;
        Texture* specExp = NULL;
    };

    // Create a new Mesh object without a mesh
    Mesh() { name = "NewMesh" + std::to_string(SM::unnamedMeshCount++); }

    // Create a new unnamed Mesh object
    Mesh(std::string nm) {
        name = nm;
    }

    virtual ~Mesh() = default;

    virtual bool loadMesh(std::string mesh_path) = 0;
    virtual void populateBuffers() = 0;

    std::string name;
    mat4 mat;
    vec3 dir = vec3(1, 0, 0);
    unsigned int VAO = 0;    // mesh vao
    unsigned int p_VBO = 0;  // position vbo
    unsigned int n_VBO = 0;  // normal vbo
    unsigned int t_VBO = 0;  // texture vbo
    unsigned int d_VBO = 0;  // texture depth vbo
    unsigned int EBO = 0;    // index (element) vbo (ebo)
    unsigned int IBO = 0;    // instance vbo (ibo)
    int atlasTileSize = 0, atlasTilesUsed = 1;
    bool usingAtlas = false;       // flag if the mesh is using an array texture
    bool isAtlasVertical = false;  // flag if the mesh is using a verticalarray texture

    std::vector<vec3> m_Positions;
    std::vector<vec3> m_Normals;
    std::vector<vec2> m_TexCoords;
    std::vector<unsigned int> m_Indices;
    std::vector<MeshObject> m_Meshes;
    std::vector<Texture*> m_Textures;
    Texture* texture;
    std::vector<Material> m_Materials;
};

#endif /* MESH_H */
