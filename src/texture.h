#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>

class Texture {
   public:
    Texture(GLenum);                     // create from memory buffer
    Texture(const std::string, GLenum);  // create from file
    Texture(const std::vector<std::string>, GLenum);  // create from files

    // load given file
    bool load();
    // load from files (cubemap)
    // bool load(std::vector<std::string> faces);
    bool loadCubemap(std::vector<std::string> faces);
    // load from memory buffer (embedded textures)
    bool load(unsigned int, void*); 
    void bind(GLenum textureUnit);
    void bind(GLenum textureType, GLenum textureUnit);

    std::string file_name;
    std::vector<std::string> cubemap_file_names;
    GLenum textureEnum;
    unsigned int texture = 0;
    int _width = 0, _height = 0, _nrChannels = 0;
};

#endif /* TEXTURE_H */
