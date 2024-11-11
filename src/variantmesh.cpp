#include "variantmesh.h"

VariantMesh::~VariantMesh() {}

bool VariantMesh::loadMeshes(std::vector<VariantInfo *> infos) {
    bool valid = true;
    for (auto v : infos) {
        valid &= v->loadMesh();
        for (auto x : v->mesh->vertices) vertices.push_back(x);
        for (auto x : v->mesh->normals) normals.push_back(x);
        for (auto x : v->mesh->texCoords) texCoords.push_back(x);
        for (auto x : v->mesh->materials)
            if (x.diffTex || x.mtlsTex) materials.push_back(x);
        for (auto x : v->mesh->indices) indices.push_back(x);
        // for (auto x : v->mesh->m_Bones) m_Bones.push_back(x);
        // for (auto x : v->mesh->m_BoneInfo) m_BoneInfo.push_back(x);
        for (auto x : v->depths) depths.push_back((float)x);
        paths.push_back(v->path);
        totalInstanceCount += v->instanceCount;
    }
    printf("%s: total instance count: %d\n", name.c_str(), totalInstanceCount);
    return valid && initScene();
}

bool VariantMesh::initScene() {
    populateBuffers();
    return glGetError() == GL_NO_ERROR;
}

void VariantMesh::populateBuffers() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &p_VBO);
    glGenBuffers(1, &n_VBO);
    glGenBuffers(1, &t_VBO);
    glGenBuffers(1, &d_VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &IBO);
    glGenBuffers(1, &BBO);
    glGenBuffers(1, &BTBO);

    glGenBuffers(1, &commandBuffer);
    // glNamedBufferStorage(commandBuffer, sizeof(IndirectDrawCommand) * cmds.size(), (const void *)cmds.data(), GL_DYNAMIC_STORAGE_BIT); // for compute shaders

    glBindBuffer(GL_ARRAY_BUFFER, p_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(VA_POSITION_LOC);
    glVertexAttribPointer(VA_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, n_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(VA_NORMAL_LOC);
    glVertexAttribPointer(VA_NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, t_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords[0]) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(VA_TEXTURE_LOC);
    glVertexAttribPointer(VA_TEXTURE_LOC, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // glBindBuffer(GL_ARRAY_BUFFER, BBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(m_Bones[0]) * m_Bones.size(), &m_Bones[0], GL_STATIC_DRAW);
    // glEnableVertexAttribArray(VA_BONE_LOC);
    // glVertexAttribIPointer(VA_BONE_LOC, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (const GLvoid *)0);
    // glEnableVertexAttribArray(VA_BONE_WEIGHT_LOC);
    // glVertexAttribPointer(VA_BONE_WEIGHT_LOC, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE,
    //                       sizeof(VertexBoneData), (const GLvoid *)(MAX_NUM_BONES_PER_VERTEX * sizeof(unsigned int)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    for (unsigned int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(VA_INSTANCE_LOC + i);
        glVertexAttribPointer(VA_INSTANCE_LOC + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (const void *)(i * sizeof(vec4)));
        glVertexAttribDivisor(VA_INSTANCE_LOC + i, 1);  // tell OpenGL this is an instanced vertex attribute.
    }

    glBindBuffer(GL_ARRAY_BUFFER, d_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(depths[0]) * depths.size(), &depths[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(VA_DEPTH_LOC);
    glVertexAttribPointer(VA_DEPTH_LOC, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
    glVertexAttribDivisor(VA_DEPTH_LOC, 1);  // tell OpenGL this is an instanced vertex attribute.
    generateCommands();
}

void VariantMesh::generateCommands() {
    IndirectDrawCommand cmds[variants.size()];
    unsigned int baseVertex = 0, baseInstance = 0, baseIndex = 0;
    int i = 0;
    for (const auto &v : variants) {
        int vCount = v->mesh->vertices.size();  // vertices in this mesh
        int iCount = v->mesh->indices.size();    // indices in thie mesh

        cmds[i].indexCount = iCount;
        cmds[i].instanceCount = v->instanceCount;  // number of instances this mesh will have
        cmds[i].baseIndex = baseIndex;
        cmds[i].baseVertex = baseVertex;
        cmds[i].baseInstance = baseInstance;  // index to begin new set of mesh instances

        baseVertex += vCount;
        baseIndex += iCount;
        baseInstance += v->instanceCount;
        i++;
    }

    // send command buffers to gpu
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(cmds), cmds, GL_DYNAMIC_DRAW);
}

void VariantMesh::loadMaterials() {
    for (int i = 0; i < variants.size(); ++i) {
        auto diff_id = GL_TEXTURE0 + i * 2;
        glActiveTexture(diff_id);
        if (variants[i]->mesh->materials[1].diffTex) glBindTexture(GL_TEXTURE_2D_ARRAY, variants[i]->mesh->materials[1].diffTex->texture);

        auto mtl_id = GL_TEXTURE0 + (i * 2) + 1;
        glActiveTexture(mtl_id);
        if (variants[i]->mesh->materials[1].mtlsTex) glBindTexture(GL_TEXTURE_2D_ARRAY, variants[i]->mesh->materials[1].mtlsTex->texture);
    }
}

// unbind textures so they don't "spill over"
void VariantMesh::unloadMaterials() {
    for (int i = 0; i < variants.size(); ++i) {
        auto diff_id = GL_TEXTURE0 + i * 2;
        glActiveTexture(diff_id);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        auto mtls_id = GL_TEXTURE0 + (i * 2) + 1;
        glActiveTexture(mtls_id);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
}

void VariantMesh::render(const mat4 *bone_trans_matrix) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * totalInstanceCount, &bone_trans_matrix[0], GL_DYNAMIC_DRAW);

    loadMaterials();

    glMultiDrawElementsIndirect(
        GL_TRIANGLES,     // draw triangles
        GL_UNSIGNED_INT,  // data type in indices
        (const void *)0,  // no offset; commands already bound to buffer
        variants.size(),  // number of variants
        0                 // no stride
    );

    unloadMaterials();

    glBindVertexArray(0);  // prevent VAO from being changed externally
}

void VariantMesh::render(mat4 mm) {
    this->mat = mm;
    render(&mm);
}

void VariantMesh::update(Shader *skinnedShader) {
    // for (const auto &v : variants) {
    //     v->mesh->update(skinnedShader);
    // }
}

void VariantMesh::update() {
    // for (const auto &v : variants) {
    //     v->mesh->update();
    // }
    // for (int i = 0; i < 200; i++) {
    //     shader->setMat4("bones[" + std::to_string(i) + "]", mat4(1));
    // }
}

void VariantMesh::update(std::vector<float> speeds) {
    assert(speeds.size() == variants.size());
    // for (const auto &v : variants) {
    //     v->mesh->update();
    // }
    // for (int i = 0; i < 200; i++) {
    //     shader->setMat4("bones[" + std::to_string(i) + "]", mat4(1));
    // }
}
