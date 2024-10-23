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
    bool load();                                      // load given file
    bool load(std::vector<std::string> faces);        // load from files
    bool load(unsigned int, void*);                   // load from memory buffer
    void bind(GLenum textureUnit);
    void bind(GLenum textureType, GLenum textureUnit);

    std::string file_name;
    std::vector<std::string> cubemap_file_names;
    GLenum textureEnum;
    unsigned int texture = 0;
    int _width = 0, _height = 0, _nrChannels = 0;
};

#endif /* TEXTURE_H */
