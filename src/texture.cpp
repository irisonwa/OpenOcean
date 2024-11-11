// #pragma warning(disable : 26495)
#include "texture.h"
#include "stb/stb_image.h"
#define STB_IMAGE_IMPLEMENTATION

void Texture::bind(GLenum textureUnit) {
    glActiveTexture(textureUnit);
    glBindTexture(textureEnum, texture);  // bind model's texture
}

bool Texture::_loadAtlas(std::string path, int tileSize, int tiles) {
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &_width, &_height, &_nrChannels, 0);
    if (data) {
        if (textureEnum == GL_TEXTURE_2D_ARRAY) {
            glGenTextures(1, &texture);
            glBindTexture(textureEnum, texture);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // align data to 1-byte. used to prevent warping of certain textures.
            GLint bpp = 0;
            switch (_nrChannels) {
                case 1:
                    bpp = GL_RED;
                    break;
                case 3:
                    bpp = GL_RGB;
                    break;
                case 4:
                    bpp = GL_RGBA;
                    break;
                default:
                    printf("unsupported image bits per pixel");
                    stbi_image_free(data);
                    return glGetError() == GL_NO_ERROR;
            }
            int tsz = tileSize == -1 ? _width : tileSize;
            int ts = tiles < 1 ? 1 : tiles;

            // texture arrays
            glTexImage3D(textureEnum, 0, bpp, tsz, tsz, ts, 0, bpp, GL_UNSIGNED_BYTE, data);
            glTexParameteri(textureEnum, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(textureEnum, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            // set the texture wrapping/filtering options (on the currently bound texture object)
            glTexParameteri(textureEnum, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(textureEnum, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        } else {
            printf("Texture type %x is not supported.", textureEnum);
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
            exit(1);  // exit if trying to load different texture type
        }
    } else {
        std::cout << "Failed to load texture " << path.c_str() << std::endl;
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    return glGetError() == GL_NO_ERROR;
}

bool Texture::loadAtlas(std::string path, int tileSize = -1, int tiles = -1) {
    file_names.push_back(path);
    return _loadAtlas(path, tileSize, tiles);
}

bool Texture::loadAtlas(int tileSize = -1, int tiles = -1) {
    return _loadAtlas(file_name, tileSize, tiles);
}

bool Texture::load() {
    return load(file_name);
}

bool Texture::load(std::string tex) {
    glGenTextures(1, &texture);
    // load and generate the texture
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(tex.c_str(), &_width, &_height, &_nrChannels, 0);
    if (data) {
        if (textureEnum == GL_TEXTURE_2D) {
            glBindTexture(textureEnum, texture);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // align data to 1-byte. used to prevent warping of certain textures.
            GLint bpp = 0;
            switch (_nrChannels) {
                case 1:
                    bpp = GL_RED;
                    break;
                case 3:
                    bpp = GL_RGB;
                    break;
                case 4:
                    bpp = GL_RGBA;
                    break;
                default:
                    printf("unsupported image bits per pixel");
                    break;
            }
            glTexImage2D(textureEnum, 0, bpp, _width, _height, 0, bpp, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(textureEnum);
            // set the texture wrapping/filtering options (on the currently bound texture object)
            glTexParameteri(textureEnum, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(textureEnum, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(textureEnum, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(textureEnum, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            printf("Texture type %x is not supported.", textureEnum);
            exit(1);  // exit if trying to load different texture type
        }
    } else {
        std::cout << "Failed to load texture " << tex.c_str() << std::endl;
    }
    stbi_image_free(data);
    // glBindTexture(GL_TEXTURE_2D, 0);
    return glGetError() == GL_NO_ERROR;
}

bool Texture::loadCubemap(std::vector<std::string> faces) {
    // glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    // load and generate the texture
    for (int i = 0; i < faces.size(); i++) {
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(faces[i].c_str(), &_width, &_height, &_nrChannels, 0);
        if (data) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // align data to 1-byte. used to prevent warping of certain textures.
            switch (_nrChannels) {
                case 1:
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, _width, _height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
                    break;
                case 3:
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                    break;
                case 4:
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                    break;
                default:
                    printf("unsupported image bits per pixel");
                    break;
            }
            stbi_image_free(data);
            // std::cout << "Cubemap: Loaded texture " << faces[i] << std::endl;
        } else {
            std::cout << "Failed to load texture " << faces[i] << std::endl;
            std::cout << stbi_failure_reason() << std::endl;
            stbi_image_free(data);
            return false;
        }
    }
    // set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);     // minification
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);     // magnification
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // u coordinate (x)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  // v coordinate (y)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  // w coordinate (z)
    return glGetError() == GL_NO_ERROR;
}

bool Texture::load(unsigned int buffer, void* img_data) {
    void* data = stbi_load_from_memory((const stbi_uc*)img_data, buffer, &_width, &_height, &_nrChannels, 0);
    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &texture);
    glBindTexture(textureEnum, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // align data to 1-byte. used to prevent warping of certain textures.
    switch (_nrChannels) {
        case 1:
            glTexImage2D(textureEnum, 0, GL_RED, _width, _height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
            break;
        case 3:
            glTexImage2D(textureEnum, 0, GL_RGB8, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            break;
        case 4:
            glTexImage2D(textureEnum, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            break;
        default:
            printf("unsupported image bits per pixel");
            stbi_image_free(data);
            return false;
    }

    // set texture parameters
    glTexParameteri(textureEnum, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // minification
    glTexParameteri(textureEnum, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // magnification
    glTexParameteri(textureEnum, GL_TEXTURE_WRAP_S, GL_REPEAT);      // u coordinate (x)
    glTexParameteri(textureEnum, GL_TEXTURE_WRAP_T, GL_REPEAT);      // v coordinate (y)
    return glGetError() == GL_NO_ERROR;
}
