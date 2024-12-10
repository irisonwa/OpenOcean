#include "variantmesh.h"

VariantMesh::~VariantMesh() {}

bool VariantMesh::loadMeshes(std::vector<VariantInfo *> infos) {
    bool valid = true;
    boneTransformOffsets.push_back(0);
    for (auto v : infos) {
        valid &= v->loadMesh();
        for (auto x : v->mesh->vertices) vertices.push_back(x);
        for (auto x : v->mesh->normals) normals.push_back(x);
        for (auto x : v->mesh->texCoords) texCoords.push_back(x);
        for (auto x : v->mesh->materials) {
            if (x.diffTex || x.mtlsTex) materials.push_back(x);
        }
        for (auto x : v->mesh->indices) indices.push_back(x);
        for (auto x : v->depths) depths.push_back((float)x);
        for (auto x : v->mesh->vBones) vBones.push_back(x);
        for (auto x : v->mesh->boneInfos) boneInfos.push_back(x);
        for (auto x : v->mesh->animations) animations.push_back(x);
        paths.push_back(v->path);
        globalInverseMatrices.push_back(Util::aiToGLM(&v->mesh->globalInverseTrans));
        boneTransformOffsets.push_back(boneInfos.size());
        totalInstanceCount += v->instanceCount;
    }
    if (vBones.empty()) vBones.resize(totalInstanceCount);
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * SM::MAX_NUM_BOIDS, NULL, GL_DYNAMIC_DRAW);
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

    if (type == SKINNED) {
        glBindBuffer(GL_ARRAY_BUFFER, BBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vBones[0]) * vBones.size(), &vBones[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(VA_BONE_LOC);
        glVertexAttribIPointer(VA_BONE_LOC, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (const GLvoid *)0);
        glEnableVertexAttribArray(VA_BONE_WEIGHT_LOC);
        glVertexAttribPointer(VA_BONE_WEIGHT_LOC, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE,
                              sizeof(VertexBoneData), (const GLvoid *)(MAX_NUM_BONES_PER_VERTEX * sizeof(unsigned int)));

        // ssbos
        glCreateBuffers(1, &ABBO);
        glCreateBuffers(1, &BIBO);
        glCreateBuffers(1, &BOBO);
        auto bufflag = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
        glNamedBufferStorage(ABBO, animations.size() * sizeof(Animation), animations.data(), bufflag);
        glNamedBufferStorage(BIBO, boneInfos.size() * sizeof(BoneInfo), boneInfos.data(), bufflag);
        glNamedBufferStorage(BOBO, boneTransformOffsets.size() * sizeof(int), boneTransformOffsets.data(), bufflag);
    }
    generateCommands();
}

void VariantMesh::generateCommands() {
    glGenBuffers(1, &commandBuffer);

    IndirectDrawCommand *cmds = new IndirectDrawCommand[variants.size()];
    unsigned int baseVertex = 0, baseInstance = 0, baseIndex = 0;
    for (int i = 0; i < variants.size(); ++i) {
        const auto &v = variants[i];
        int vCount = v->mesh->vertices.size();  // vertices in this mesh
        int iCount = v->mesh->indices.size();   // indices in thie mesh

        cmds[i].indexCount = iCount;
        cmds[i].instanceCount = v->instanceCount;  // number of instances this mesh will have
        cmds[i].baseIndex = baseIndex;
        cmds[i].baseVertex = baseVertex;
        cmds[i].baseInstance = baseInstance;  // index to begin new set of mesh instances

        baseVertex += vCount;
        baseIndex += iCount;
        baseInstance += v->instanceCount;
    }

    // send command buffers to gpu
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(IndirectDrawCommand) * variants.size(), &cmds[0], GL_DYNAMIC_DRAW);
}

// Bind up to 12 diffuse and metalness textures
void VariantMesh::loadMaterials() {
    for (int i = 0; i < variants.size(); ++i) {
        auto diff_id = GL_TEXTURE0 + i * 2;
        glActiveTexture(diff_id);
        if (materials[i].diffTex) glBindTexture(GL_TEXTURE_2D_ARRAY, materials[i].diffTex->texture);

        auto mtl_id = GL_TEXTURE0 + (i * 2) + 1;
        glActiveTexture(mtl_id);
        if (materials[i].mtlsTex) glBindTexture(GL_TEXTURE_2D_ARRAY, materials[i].mtlsTex->texture);
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

// Update and render all animations for each variant
void VariantMesh::render(const mat4 *instance_trans_matrix) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);  // rebind command buffer
    if (type == SKINNED) {
#ifdef TREE
        glBindBuffer(GL_ARRAY_BUFFER, IBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mat4) * totalInstanceCount, &instance_trans_matrix[0]);
#endif
        // update animations
        animShader->use();
        animShader->setFloat("timeSinceApplicationStarted", SM::getGlobalTime());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ABBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, BIBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, BOBO);
        glDispatchCompute((int)ceil(boneInfos.size() / 32.f), 1, 1);  // declare work group sizes and run compute shader
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);               // wait for all threads to be finished
    } else if (type == STATIC) {
        glBindBuffer(GL_ARRAY_BUFFER, IBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mat4) * totalInstanceCount, &instance_trans_matrix[0]);
    }

    loadMaterials();

    shader->use();
    glMultiDrawElementsIndirect(
        GL_TRIANGLES,     // draw triangles
        GL_UNSIGNED_INT,  // data type in indices
        (const void *)0,  // no offset; commands already bound to buffer
        variants.size(),  // number of variants
        0                 // no stride
    );

    unloadMaterials();
    glUseProgram(0);
    glBindVertexArray(0);  // prevent VAO from being changed externally
}

void VariantMesh::render(mat4 mm) {
    this->mat = mm;
    render(&mm);
}

void VariantMesh::render() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);  // rebind command buffer

    // update animations
    animShader->use();
    animShader->setFloat("timeSinceApplicationStarted", SM::getGlobalTime());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ABBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, BIBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, BOBO);
    glDispatchCompute((int)ceil(boneInfos.size() / 32.f), 1, 1);  // declare work group sizes and run compute shader
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);               // wait for all threads to be finished

    loadMaterials();

    shader->use();
    glMultiDrawElementsIndirect(
        GL_TRIANGLES,     // draw triangles
        GL_UNSIGNED_INT,  // data type in indices
        (const void *)0,  // no offset; commands already bound to buffer
        variants.size(),  // number of variants
        0                 // no stride
    );

    unloadMaterials();
    glUseProgram(0);
    glBindVertexArray(0);  // prevent VAO from being changed externally
}
