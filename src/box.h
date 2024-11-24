#ifndef BOX_H
#define BOX_H

#include "sm.h"
#include "camera.h"
#include "util.h"
#include "shader.h"

class Box {
public:
    Box() {
        low = vec3(0);
        high = vec3(0);
        size = vec3(0);
        centre = vec3(0);
        transform = scale(translate(mat4(1), centre), size);
        // loadWireframe();
    }

    Box(vec3 l, vec3 h) {
        low = l;
        high = h;
        size = h - l;
        centre = low + size / 2.f;
        transform = scale(translate(mat4(1), centre), size);
        // loadWireframe();
    }
    
    Box(std::vector<vec3> points) : Box() {
        for (const auto& p : points) {
            grow(p);
        }
        transform = scale(translate(mat4(1), centre), size);
        // loadWireframe();
    }

    ~Box() {
        // glBindVertexArray(0);
        // glDeleteBuffers(1, &VAO);
        // glDeleteBuffers(1, &VBO);
    }

    // split the box into 8 octants
    std::vector<Box> split() {
        std::vector<Box> children = {
            Box(low, centre),
            Box(vec3(low.x, low.y, centre.z), vec3(centre.x, centre.y, high.z)),
            Box(vec3(low.x, centre.y, low.z), vec3(centre.x, high.y, centre.z)),
            Box(vec3(low.x, centre.y, centre.z), vec3(centre.x, high.y, high.z)),
            Box(centre, high),
            Box(vec3(centre.x, low.y, low.z), vec3(high.x, centre.y, centre.z)),
            Box(vec3(centre.x, low.y, centre.z), vec3(high.x, centre.y, high.z)),
            Box(vec3(centre.x, centre.y, low.z), vec3(high.x, high.y, centre.z)),
        };
        return children;
    }

    // get the index (0-7) of the octant containing the point `pos`
    int getOctant(vec3 pos) {
        int msk = 0;
        if (pos.x >= centre.x) msk |= 4;
        if (pos.y >= centre.y) msk |= 2;
        if (pos.z >= centre.z) msk |= 1;
        // msk += pos.x > centre.x ? 4 : 0;
        // msk += pos.y > centre.y ? 2 : 0;
        // msk += pos.z > centre.z ? 1 : 0;
        return msk;
    }

    // does the box contain this point?
    bool contains(vec3 p) {
        return p.x <= high.x && p.x >= low.x &&
               p.y <= high.y && p.y >= low.y &&
               p.z <= high.z && p.z >= low.z;
    }

    // give the box a point. if the point is outside the box's bounds, the box will grow to include the point. returns true if the box grew
    bool grow(vec3 p) {
        if (contains(p)) return false;
        if (p.x > high.x) high.x = p.x;
        if (p.x < low.x) low.x = p.x;
        if (p.y > high.y) high.y = p.y;
        if (p.y < low.y) low.y = p.y;
        if (p.z > high.z) high.z = p.z;
        if (p.z < low.z) low.z = p.z;
        size = high - low;
        centre = low + size / 2.f;
        return true;
    }

    // get the corners of the box
    std::vector<vec3> points() {
        std::vector<vec3> ps = {
            low,
            vec3(low.x, high.y, low.z),
            vec3(high.x, low.y, low.z),
            vec3(high.x, high.y, low.z),
            high,
            vec3(low.x, low.y, high.z),
            vec3(high.x, low.y, high.z),
            vec3(low.x, high.y, high.z)
        };
        return ps;
    }

    void loadWireframe() {
        shader = new Shader("wire", PROJDIR "Shaders/blank.vert", PROJDIR "Shaders/blank.frag");
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

        // glGenBuffers(1, &EBO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
        
        glBindVertexArray(0);
    }

    void drawWireframe() {
        glDisable(GL_CULL_FACE);
        shader->use();
        shader->setMat4("proj", SM::camera->getProjectionMatrix());
        shader->setMat4("view", SM::camera->getViewMatrix());
        shader->setMat4("model", transform);
        glBindVertexArray(VAO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
        // glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
        // glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // show wireframe of bounding box
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // show full faces
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);
    }

    vec3 low;     // bottom left position
    vec3 high;    // top right position
    vec3 size;    // diagonal size of box (high - low)
    vec3 centre;  // centre point of box (low + size / 2)

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    mat4 transform;
    Shader *shader;
    std::vector<float> vertices = {
        // positions
        // https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/2.2.basic_lighting_specular/basic_lighting_specular.cpp
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f};

    std::vector<int> indices = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7};
};


// Struct vertion of Box class. Suitable for shaders
struct BBox {
    BBox() {
        low = vec4(0);
        high = vec4(0);
        size = vec4(0);
        centre = vec4(0);
    }
    BBox(Box b) {
        low = vec4(b.low, 0);
        high = vec4(b.high, 0);
        size = vec4(b.size, 0);
        centre = vec4(b.centre, 0);
    }
    vec4 low;     // bottom left position
    vec4 high;    // top right position
    vec4 size;    // diagonal size of box (high - low)
    vec4 centre;  // centre point of box (low + size / 2)
};


#endif /* BOX_H */
