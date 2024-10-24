#include "cubemap.h"

void Cubemap::loadCubemap(std::vector<std::string> fs) {
    // bind vao and vbo
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);

    // bind texture
    tex = new Texture(fs, GL_TEXTURE_CUBE_MAP);
    bool loaded = tex->loadCubemap(fs);
    assert(loaded);  // make sure the cubemap's textures have fully loaded

    // fill vbo data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * skyboxVertices.size(), &skyboxVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(POSITION_LOC);
}

void Cubemap::render() {
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex->texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
