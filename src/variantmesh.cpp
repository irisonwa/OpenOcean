#include "variantmesh.h"

VariantMesh::~VariantMesh() {}

bool VariantMesh::loadMeshes(std::vector<std::string> mesh_paths) {
    bool valid = true;
    for (auto path : mesh_paths) {
        BoneMesh* bm = new BoneMesh();
        valid &= bm->loadMesh(path, false);
        meshes.push_back(bm);
    }
    return valid;
}

bool VariantMesh::initScene() {
        populateBuffers();
    return glGetError() == GL_NO_ERROR;
}

void VariantMesh::populateBuffers() {
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

    // todo: bones, bone weights

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    for (unsigned int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(VA_INSTANCE_LOC + i);
        glVertexAttribPointer(VA_INSTANCE_LOC + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (const void*)(i * sizeof(vec4)));
        glVertexAttribDivisor(VA_INSTANCE_LOC + i, 1);  // tell OpenGL this is an instanced vertex attribute.
    }
}

void VariantMesh::render(unsigned int nInstances, const mat4* bone_trans_matrix) {
    mat = bone_trans_matrix[0];
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * nInstances, &bone_trans_matrix[0], GL_DYNAMIC_DRAW);

    for (unsigned int i = 0; i < m_Meshes.size(); i++) {
        unsigned int mIndex = m_Meshes[i].materialIndex;
        assert(mIndex < m_Materials.size());

        if (m_Materials[mIndex].diffTex) m_Materials[mIndex].diffTex->bind(GL_TEXTURE0);
        if (m_Materials[mIndex].specExp) m_Materials[mIndex].specExp->bind(GL_TEXTURE1);
        glDrawElementsInstancedBaseVertex(
            GL_TRIANGLES,
            m_Meshes[i].n_Indices,
            GL_UNSIGNED_INT,
            (void*)(sizeof(unsigned int) * m_Meshes[i].baseIndex),
            nInstances,
            m_Meshes[i].baseVertex);
    }
    glBindVertexArray(0);  // prevent VAO from being changed externally
}

void VariantMesh::render(mat4 mm) {
    this->mat = mm;
    render(1, &mm);
}

void VariantMesh::update(Shader* skinnedShader) {
    for (auto bm : meshes) {
        std::vector<aiMatrix4x4> trans;
        float animTime = ((float)(timeGetTime() - SM::startTime)) / 1000.0f;
        bm->getBoneTransforms(animTime, trans);
        for (int i = 0; i < trans.size(); i++) {
            mat4 t = Util::aiToGLM(&trans[i]);
            skinnedShader->setMat4("bones[" + std::to_string(i) + "]", t);
        }
    }
}

void VariantMesh::update() {
    for (auto bm : meshes) {
        bm->update();
    }
}
