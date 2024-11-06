#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>

#include "util.h"

class Texture {
   private:
    bool _loadAtlas(std::string tex, int tileSize, int tiles);

   public:
    // create from memory buffer
    Texture(GLenum texType = GL_TEXTURE_2D) {
        textureEnum = texType;
        isAtlas = texType == GL_TEXTURE_2D_ARRAY;
    }

    // create from file
    Texture(const std::string fname, GLenum texType = GL_TEXTURE_2D) {
        textureEnum = texType;
        file_name = fname;
        isAtlas = texType == GL_TEXTURE_2D_ARRAY;
    }

    // create from files
    Texture(const std::vector<std::string> fnames, GLenum texType = GL_TEXTURE_CUBE_MAP) {
        textureEnum = texType;
        file_names = fnames;
    }
    
    // load the file given in the constructor
    bool load();

    // load the file `tex`
    bool load(std::string tex);

    // load the diffuse texture `diffuseTex` and the specular map `specularTex`
    // bool load(std::string diffuseTex, std::string specularTex);

    // load an atlas of textures. 
    // `tiles` is the number of actual, non-empty tiles in the atlas, and `tileSize` is the width (and height) of a single tile.
    bool loadAtlas(std::string tex, int tileSize, int tiles);

    // load the atlas of textures given in the constructor. 
    // `tiles` is the number of actual, non-empty tiles in the atlas, and `tileSize` is the width (and height) of a single tile.
    bool loadAtlas(int tileSize, int tiles);

    // load from files (cubemap)
    bool loadCubemap(std::vector<std::string> faces);

    // load from memory buffer (embedded textures)
    bool load(unsigned int, void*);

    // bind all available textures. this depends on the number of materials (e.g., material slots in Blender) in the mesh if it's an atlas texture.
    // void bind();

    // bind a single texture
    void bind(GLenum textureUnit);

    // bind a diffuse and specular texture
    // void bind(GLenum textureType, GLenum textureUnit);

    std::string file_name;
    std::vector<std::string> file_names;
    GLenum textureEnum;
    unsigned int texture = 0;
    int _width = 0, _height = 0, _nrChannels = 0;
    bool isAtlas = false;
};

#endif /* TEXTURE_H */
