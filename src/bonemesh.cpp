#include "bonemesh.h"

BoneMesh::~BoneMesh() {}

// load a mesh located at `mesh_path`. can optionally disable populating shader buffers.
bool BoneMesh::loadMesh(std::string mesh_name, bool popBuffers) {
    populateBuffer = popBuffers;

    std::string rpath = MODELDIR(mesh_name) + mesh_name;
    scene = importer.ReadFile(rpath.c_str(), B_AI_LOAD_FLAGS);

    bool valid_scene = false;

    if (!scene) {
        fprintf(stderr, "ERROR: reading mesh %s\n%s", rpath.c_str(), importer.GetErrorString());
        valid_scene = false;
    } else {
        globalInverseTrans = scene->mRootNode->mTransformation.Inverse();  // invert global transformation matrix
        valid_scene = initScene(scene, mesh_name);
        if (!valid_scene) {
            fprintf(stderr, "ERROR: reading mesh %s\n%s", rpath.c_str(), importer.GetErrorString());
        }
    }

    if (valid_scene) printf("Successfully loaded %sboned mesh \"%s\"\n", popBuffers ? "" : "variant ", name.c_str());
    return valid_scene;
}

bool BoneMesh::initScene(const aiScene* scene, std::string mesh_name) {
    meshes.resize(scene->mNumMeshes);
    materials.resize(scene->mNumMaterials);

    unsigned int nvertices = 0;
    unsigned int nindices = 0;

    // Count all vertices and indices
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].materialIndex = scene->mMeshes[i]->mMaterialIndex;  // get current material index
        meshes[i].n_Indices = scene->mMeshes[i]->mNumFaces * 3;       // there are 3 times as many indices as there are faces (since they're all triangles)
        meshes[i].baseVertex = nvertices;                             // index of first vertex in the current mesh
        meshes[i].baseIndex = nindices;                               // track number of indices

        // Move forward by the corresponding number of vertices/indices to find the base of the next vertex/index
        nvertices += scene->mMeshes[i]->mNumVertices;
        nindices += meshes[i].n_Indices;
    }

    // Reallocate space for structure of arrays (SOA) values
    vertices.reserve(nvertices);
    normals.reserve(nvertices);
    texCoords.reserve(nvertices);
    indices.reserve(nindices);
    m_Bones.resize(nvertices);

    // Initialise meshes
    for (unsigned int i = 0; i < meshes.size(); i++) {
        const aiMesh* am = scene->mMeshes[i];
        initSingleMesh(i, am);
    }

    if (!initMaterials(scene, mesh_name)) {
        return false;
    }

    if (populateBuffer) {
        populateBuffers();
    }
    return glGetError() == GL_NO_ERROR;
}

void BoneMesh::initSingleMesh(unsigned int mIndex, const aiMesh* amesh) {
    // Populate the vertex attribute vectors
    for (unsigned int i = 0; i < amesh->mNumVertices; i++) {
        const aiVector3D& pPos = amesh->mVertices[i];
        const aiVector3D& pNormal = amesh->mNormals ? amesh->mNormals[i] : aiVector3D(0.0f, 1.0f, 0.0f);
        const aiVector3D& pTexCoord = amesh->HasTextureCoords(0) ? amesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f);

        vertices.push_back(vec3(pPos.x, pPos.y, pPos.z));
        normals.push_back(vec3(pNormal.x, pNormal.y, pNormal.z));
        texCoords.push_back(vec2(pTexCoord.x, pTexCoord.y));
    }

    // Load all bones in mesh
    for (int i = 0; i < amesh->mNumBones; i++) {
        aiBone* aBone = amesh->mBones[i];
        unsigned int boneID = getBoneID(aBone);

        for (int j = 0; j < aBone->mNumWeights; j++) {
            unsigned int gvID = meshes[mIndex].baseVertex + aBone->mWeights[j].mVertexId;  // global vertex id
            float weight = aBone->mWeights[j].mWeight;
            m_Bones[gvID].addBoneData(boneID, weight);
        }
    }

    // Populate the index buffer
    for (unsigned int i = 0; i < amesh->mNumFaces; i++) {
        const aiFace& Face = amesh->mFaces[i];
        for (unsigned int i = 0; i < Face.mNumIndices; i++) {
            indices.push_back(Face.mIndices[i]);
        }
    }
}

int BoneMesh::getBoneID(const aiBone* pBone) {
    std::string bName = pBone->mName.C_Str();

    if (boneToIndexMap.find(bName) == boneToIndexMap.end()) {
        int bIndex = boneToIndexMap.size();
        BoneInfo* bi = new BoneInfo();
        m_BoneInfo.push_back(bi);
        m_BoneInfo[bIndex]->offsetMatrix = pBone->mOffsetMatrix;
        boneToIndexMap[bName] = bIndex;
        return bIndex;
    } else {
        return boneToIndexMap[bName];
    }
}

std::vector<aiMatrix4x4> BoneMesh::getBoneTransforms(float timeSinceStarted, float animSpeed) {
    std::vector<aiMatrix4x4> trans(m_BoneInfo.size(), aiMatrix4x4());
    float tps = 0.0f;
    float animTime = 0.0f;
    if (scene->HasAnimations()) {
        /* mAnimations[0] -> the first animation. change the index to access other animations */
        tps = 24 * animSpeed;                                                              // ticks per second = tps
        animTime = fmod(timeSinceStarted * tps, (float)scene->mAnimations[0]->mDuration);  // animation time in tps. modf used for looping
    }

    readNodeHierarchy(animTime, scene->mRootNode, aiMatrix4x4());

    for (int i = 0; i < m_BoneInfo.size(); i++) {
        trans[i] = m_BoneInfo[i]->lastTransformation;
    }

    return trans;
}

void BoneMesh::readNodeHierarchy(float atime, const aiNode* node, const aiMatrix4x4& parent) {
    std::string nodeName(node->mName.data);
    aiMatrix4x4 nodeTrans = node->mTransformation;

    if (scene->HasAnimations()) {
        const aiAnimation* anim = scene->mAnimations[0];
        const aiNodeAnim* animNode = findNodeAnim(anim, nodeName);

        // Interpolate translation, scaling, and rotation
        if (animNode) {
            // Interpolate translation
            aiVector3D trans = calcInterpolatedTranslation(atime, animNode);
            aiMatrix4x4 transM;
            aiMatrix4x4::Translation(trans, transM);

            // Interpolate scaling
            aiVector3D scale = calcInterpolatedScale(atime, animNode);
            aiMatrix4x4 scaleM;
            aiMatrix4x4::Scaling(scale, scaleM);

            // Interpolate rotation
            aiQuaternion rotation = calcInterpolatedRotation(atime, animNode);
            aiMatrix4x4 rotM;
            rotM = aiMatrix4x4(rotation.GetMatrix());

            nodeTrans = transM * rotM * scaleM;
        }
    }

    aiMatrix4x4 globalTrans = parent * nodeTrans;
    if (boneToIndexMap.find(nodeName) != boneToIndexMap.end()) {
        unsigned int bIndex = boneToIndexMap[nodeName];
        m_BoneInfo[bIndex]->lastTransformation =
            globalInverseTrans *
            globalTrans *
            m_BoneInfo[bIndex]->offsetMatrix;
    }

    for (int i = 0; i < node->mNumChildren; i++) {
        readNodeHierarchy(atime, node->mChildren[i], globalTrans);
    }
}

const aiNodeAnim* BoneMesh::findNodeAnim(const aiAnimation* anim, const std::string nodeName) {
    for (int i = 0; i < anim->mNumChannels; i++) {
        const aiNodeAnim* tnode = anim->mChannels[i];
        if (std::string(tnode->mNodeName.data) == nodeName) {
            return tnode;
        }
    }
    return NULL;
}

aiVector3D BoneMesh::calcInterpolatedTranslation(float atime, const aiNodeAnim* node) {
    if (node->mNumPositionKeys <= 1) {
        return node->mPositionKeys[0].mValue;
    }

    // get position index
    unsigned int positionIndex = 0;
    for (unsigned int i = 0; i < node->mNumPositionKeys - 1; i++) {
        float t = (float)node->mPositionKeys[i + 1].mTime;
        if (atime < t) {
            positionIndex = i;
            break;
        }
    }
    assert(positionIndex + 1 < node->mNumPositionKeys);

    float deltaTime = node->mPositionKeys[positionIndex + 1].mTime - node->mPositionKeys[positionIndex].mTime;  // delta between two adjacent Position keyfraes
    float factor = (atime - node->mPositionKeys[positionIndex].mTime) / deltaTime;                              // intermediate position factor

    const aiVector3D& start = node->mPositionKeys[positionIndex].mValue;    // start position value
    const aiVector3D& end = node->mPositionKeys[positionIndex + 1].mValue;  // end position value
    return start + factor * (end - start);                                  // interpolated position
}

aiVector3D BoneMesh::calcInterpolatedScale(float atime, const aiNodeAnim* node) {
    if (node->mNumScalingKeys <= 1) {
        return node->mScalingKeys[0].mValue;
    }

    unsigned int scalingIndex = 0;
    for (unsigned int i = 0; i < node->mNumScalingKeys - 1; i++) {
        float t = (float)node->mScalingKeys[i + 1].mTime;
        if (atime < t) {
            scalingIndex = i;
            break;
        }
    }
    assert(scalingIndex + 1 < node->mNumScalingKeys);

    float deltaTime = node->mScalingKeys[scalingIndex + 1].mTime - node->mScalingKeys[scalingIndex].mTime;  // delta between two adjacent scaling keyfraes
    float factor = (atime - node->mScalingKeys[scalingIndex].mTime) / deltaTime;                            // intermediate scaling factor

    const aiVector3D& start = node->mScalingKeys[scalingIndex].mValue;    // start scale value
    const aiVector3D& end = node->mScalingKeys[scalingIndex + 1].mValue;  // end scale value
    return start + factor * (end - start);                                // interpolated scale
}

aiQuaternion BoneMesh::calcInterpolatedRotation(float atime, const aiNodeAnim* node) {
    if (node->mNumRotationKeys <= 1) {
        return node->mRotationKeys[0].mValue;
    }

    unsigned int rotationIndex = 0;
    for (unsigned int i = 0; i < node->mNumRotationKeys - 1; i++) {
        float t = (float)node->mRotationKeys[i + 1].mTime;
        if (atime < t) {
            rotationIndex = i;
            break;
        }
    }
    assert(rotationIndex + 1 < node->mNumRotationKeys);

    float deltaTime = node->mRotationKeys[rotationIndex + 1].mTime - node->mRotationKeys[rotationIndex].mTime;  // delta between two adjacent rotation keyfraes
    float factor = (atime - node->mRotationKeys[rotationIndex].mTime) / deltaTime;                              // intermediate rotation factor

    const aiQuaternion& start = node->mRotationKeys[rotationIndex].mValue;    // start rotation value
    const aiQuaternion& end = node->mRotationKeys[rotationIndex + 1].mValue;  // end rotation value
    aiQuaternion out;
    aiQuaternion::Interpolate(out, start, end, factor);
    return out.Normalize();  // interpolated rotation
}

bool BoneMesh::initMaterials(const aiScene* scene, std::string mesh_name) {
    std::string dir = MODELDIR(mesh_name);
    bool flag = true;
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        const aiMaterial* pMaterial = scene->mMaterials[i];
        flag &= loadDiffuseTexture(pMaterial, dir, i);
        flag &= loadSpecularTexture(pMaterial, dir, i);
    }
    return flag;
}

bool BoneMesh::loadDiffuseTexture(const aiMaterial* pMaterial, std::string dir, unsigned int index) {
    if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        aiString Path;
        if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
            const aiTexture* cTex = scene->GetEmbeddedTexture(Path.C_Str());
            if (cTex) {
                // printf("%s: embedded diffuse texture type %s\n", name.c_str(), cTex->achFormatHint);
                materials[index].diffTex = new Texture(GL_TEXTURE_2D);
                unsigned int buffer = cTex->mWidth;
                materials[index].diffTex->load(buffer, cTex->pcData);
            } else {
                std::string p(Path.data);
                if (p.substr(0, 2) == ".\\") {
                    p = p.substr(2, p.size() - 2);
                }
                std::string fullPath = dir + p;
                materials[index].diffTex = new Texture(GL_TEXTURE_2D_ARRAY);
                if (materials[index].diffTex->loadAtlas(fullPath, atlasTileSize, atlasTilesUsed)) {
                    // printf("%s: Loaded diffuse texture '%s'\n", name.c_str(), fullPath.c_str());
                } else {
                    printf("Error loading diffuse texture '%s'\n", fullPath.c_str());
                    return false;
                }
            }
        }
    }

    return glGetError() == GL_NO_ERROR;
}

bool BoneMesh::loadSpecularTexture(const aiMaterial* pMaterial, std::string dir, unsigned int index) {
    if (pMaterial->GetTextureCount(aiTextureType_METALNESS) > 0) {
        aiString Path;

        if (pMaterial->GetTexture(aiTextureType_METALNESS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
            std::string p(Path.data);
            if (p.substr(0, 2) == ".\\") {
                p = p.substr(2, p.size() - 2);
            }

            std::string fullPath = dir + p;
            materials[index].mtlsTex = new Texture(GL_TEXTURE_2D_ARRAY);
            if (materials[index].mtlsTex->loadAtlas(fullPath, atlasTileSize, atlasTilesUsed)) {
                // printf("%s: Loaded metalness texture '%s'\n", name.c_str(), fullPath.c_str());
            } else {
                printf("Error loading metalness texture '%s'\n", fullPath.c_str());
                return false;
            }
        }
    }

    return glGetError() == GL_NO_ERROR;
}

void BoneMesh::populateBuffers() {
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
    glEnableVertexAttribArray(SK_POSITION_LOC);
    glVertexAttribPointer(SK_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, n_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(SK_NORMAL_LOC);
    glVertexAttribPointer(SK_NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, t_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords[0]) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(SK_TEXTURE_LOC);
    glVertexAttribPointer(SK_TEXTURE_LOC, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, BBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Bones[0]) * m_Bones.size(), &m_Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(SK_BONE_LOC);
    glVertexAttribIPointer(SK_BONE_LOC, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(SK_BONE_WEIGHT_LOC);
    glVertexAttribPointer(SK_BONE_WEIGHT_LOC, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE,
                          sizeof(VertexBoneData), (const GLvoid*)(MAX_NUM_BONES_PER_VERTEX * sizeof(unsigned int)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    for (unsigned int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(SK_INSTANCE_LOC + i);
        glVertexAttribPointer(SK_INSTANCE_LOC + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (const void*)(i * sizeof(vec4)));
        glVertexAttribDivisor(SK_INSTANCE_LOC + i, 1);  // tell OpenGL this is an instanced vertex attribute.
    }

    glBindBuffer(GL_ARRAY_BUFFER, d_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * SM::MAX_NUM_BOIDS, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(SK_DEPTH_LOC);
    glVertexAttribPointer(SK_DEPTH_LOC, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
    glVertexAttribDivisor(SK_DEPTH_LOC, 1);  // tell OpenGL this is an instanced vertex attribute.

    glBindVertexArray(0);  // avoid modifying VAO between loads
}

void BoneMesh::render(unsigned int nInstances, const mat4* bone_trans_matrix, const float* depths) {
    mat = bone_trans_matrix[0];
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * nInstances, &bone_trans_matrix[0], GL_DYNAMIC_DRAW);

    for (unsigned int i = 0; i < meshes.size(); i++) {
        unsigned int mIndex = meshes[i].materialIndex;
        assert(mIndex < materials.size());

        glBindBuffer(GL_ARRAY_BUFFER, d_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nInstances, &depths[0], GL_DYNAMIC_DRAW);
        if (materials[mIndex].diffTex) materials[mIndex].diffTex->bind(GL_TEXTURE0);
        if (materials[mIndex].mtlsTex) materials[mIndex].mtlsTex->bind(GL_TEXTURE1);

        glDrawElementsInstancedBaseVertex(
            GL_TRIANGLES,
            meshes[i].n_Indices,
            GL_UNSIGNED_INT,
            (void*)(sizeof(unsigned int) * meshes[i].baseIndex),
            nInstances,
            meshes[i].baseVertex);
    }

    // unbind textures so they don't "spill over"
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glBindVertexArray(0);  // prevent VAO from being changed externally
}

void BoneMesh::render(unsigned int nInstances, const mat4* bone_trans_matrix) {
    std::vector<float> dpths(nInstances, 0);
    render(nInstances, bone_trans_matrix, dpths.data());
}

void BoneMesh::render(mat4 mm) {
    render(1, &mm);
}

void BoneMesh::update(Shader* skinnedShader, float animSpeed) {
    std::vector<aiMatrix4x4> trans = getBoneTransforms(SM::getGlobalTime(), animSpeed);
    for (int i = 0; i < trans.size(); i++) {
        mat4 t = Util::aiToGLM(&trans[i]);
        skinnedShader->setMat4("bones[" + std::to_string(i) + "]", t);
    }
}

void BoneMesh::update(float animSpeed) {
    std::vector<aiMatrix4x4> trans = getBoneTransforms(SM::getGlobalTime(), animSpeed);
    for (int i = 0; i < trans.size(); i++) {
        mat4 t = Util::aiToGLM(&trans[i]);
        shader->setMat4("bones[" + std::to_string(i) + "]", t);
    }
}

void BoneMesh::update() {
    update(1);
}
