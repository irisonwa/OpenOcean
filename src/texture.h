#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>

#include "util.h"

class Texture {
   public:
    Texture(GLenum);                     // create from memory buffer
    Texture(const std::string, GLenum);  // create from file
    Texture(const std::vector<std::string>, GLenum);  // create from files

    // load the given file
    bool load();
    // load an atlas of textures. `tiles` is the number of actual, non-empty tiles in the atlas, and `tileSize` is the width (and height) of a single tile.
    bool loadAtlas(std::string tex, int tiles, int tileSize);
    // load from files (cubemap)
    bool loadCubemap(std::vector<std::string> faces);
    // load from memory buffer (embedded textures)
    bool load(unsigned int, void*); 
    void bind(GLenum textureUnit);
    void bind(GLenum textureType, GLenum textureUnit);

    std::string file_name;
    std::vector<std::string> file_names;
    GLenum textureEnum;
    unsigned int texture = 0;
    int _width = 0, _height = 0, _nrChannels = 0;
};

#endif /* TEXTURE_H */
