#include "variantmesh.h"

VariantMesh::~VariantMesh() {}

bool VariantMesh::loadMeshes(std::vector<VariantInfo *> infos) {
    bool valid = true;
    for (auto v : infos) {
        valid &= v->loadMesh();
        for (auto x : v->mesh->m_Positions) m_Positions.push_back(x);
        for (auto x : v->mesh->m_Normals) m_Normals.push_back(x);
        for (auto x : v->mesh->m_TexCoords) m_TexCoords.push_back(x);
        for (auto x : v->mesh->m_Materials) m_Materials.push_back(x);
        for (auto x : v->mesh->m_Indices) m_Indices.push_back(x);
        for (auto x : v->mesh->m_Bones) m_Bones.push_back(x);
        for (auto x : v->mesh->m_BoneInfo) m_BoneInfo.push_back(x);
        for (auto x : v->depths) depths.push_back(x);
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

    glGenBuffers(1, &commandBuffer);
    // glNamedBufferStorage(commandBuffer, sizeof(IndirectDrawCommand) * cmds.size(), (const void *)cmds.data(), GL_DYNAMIC_STORAGE_BIT); // for compute shaders

    glBindBuffer(GL_ARRAY_BUFFER, p_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0]) * m_Positions.size(), &m_Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(VA_POSITION_LOC);
    glVertexAttribPointer(VA_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, n_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0]) * m_Normals.size(), &m_Normals[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(VA_NORMAL_LOC);
    glVertexAttribPointer(VA_NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, t_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0]) * m_TexCoords.size(), &m_TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(VA_TEXTURE_LOC);
    glVertexAttribPointer(VA_TEXTURE_LOC, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, BBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Bones[0]) * m_Bones.size(), &m_Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(VA_BONE_LOC);
    glVertexAttribIPointer(VA_BONE_LOC, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (const GLvoid *)0);
    glEnableVertexAttribArray(VA_BONE_WEIGHT_LOC);
    glVertexAttribPointer(VA_BONE_WEIGHT_LOC, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE,
                          sizeof(VertexBoneData), (const GLvoid *)(MAX_NUM_BONES_PER_VERTEX * sizeof(unsigned int)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    for (unsigned int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(VA_INSTANCE_LOC + i);
        glVertexAttribPointer(VA_INSTANCE_LOC + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (const void *)(i * sizeof(vec4)));
        glVertexAttribDivisor(VA_INSTANCE_LOC + i, 1);  // tell OpenGL this is an instanced vertex attribute.
    }

    glBindBuffer(GL_ARRAY_BUFFER, d_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * SM::MAX_NUM_BOIDS, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(SK_DEPTH_LOC);
    glVertexAttribPointer(SK_DEPTH_LOC, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
    glVertexAttribDivisor(SK_DEPTH_LOC, 1);  // tell OpenGL this is an instanced vertex attribute.
}

void VariantMesh::generateCommands() {
    cmds = std::vector<IndirectDrawCommand>(totalInstanceCount);
    unsigned int baseVertex = 0, baseInstance = 0, baseIndex = 0;
    int i = 0;
    for (const auto &v : variants) {
        int vCount = v->mesh->m_Positions.size();  // vertices in this mesh
        int iCount = v->mesh->m_Indices.size();    // indices in thie mesh

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

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(cmds), &cmds[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, commandBuffer);
    // keep base instance value since gl_InstanceID is reset for each mesh. this means creating our own gl_InstanceID values
    glEnableVertexAttribArray(VA_ID_LOC);
    glVertexAttribIPointer(VA_ID_LOC, 1, GL_UNSIGNED_INT, sizeof(IndirectDrawCommand), (void *)(offsetof(IndirectDrawCommand, baseInstance)));
    glVertexAttribDivisor(VA_ID_LOC, 1);  // tell OpenGL this is an instanced vertex attribute.
}

void VariantMesh::render(unsigned int nInstances, const mat4 *bone_trans_matrix) {
    std::vector<float> depths(nInstances, 0);
    glBindVertexArray(VAO);
    generateCommands();
    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * nInstances, &bone_trans_matrix[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, d_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nInstances, &depths[0], GL_DYNAMIC_DRAW);

    // TODO: figure out how this works
    // glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &data[0], nInstances, 0);

    glBindVertexArray(0);  // prevent VAO from being changed externally
}

void VariantMesh::render(mat4 mm) {
    this->mat = mm;
    render(1, &mm);
}

void VariantMesh::update(Shader *skinnedShader) {
    for (const auto& v : variants) {
        v->mesh->update(skinnedShader);
    }
}

void VariantMesh::update() {
    for (const auto &v : variants) {
        v->mesh->update();
    }
}
