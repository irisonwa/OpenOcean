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
    vBones.resize(nvertices);

    // Initialise meshes
    for (unsigned int i = 0; i < meshes.size(); i++) {
        const aiMesh* am = scene->mMeshes[i];
        initSingleMesh(i, am);
    }

    if (!initMaterials(scene, mesh_name)) {
        return false;
    }

    if (populateBuffer) populateBuffers();
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
            vBones[gvID].addBoneData(boneID, weight);
        }
    }

    // Load all animations into a common buffer of Animation structs
    animations.resize(amesh->mNumBones);
    createAnimationList(scene->mRootNode, 0, mat4(1));

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
        BoneInfo bi = BoneInfo(boneToIndexMap.size());
        bi.offsetMatrix = Util::aiToGLM(&pBone->mOffsetMatrix);
        boneInfos.push_back(bi);
        boneToIndexMap[bName] = bi.ID;
        return bi.ID;
    } else {
        return boneToIndexMap[bName];
    }
}

// Populates the `animations` buffer.
// returned value is a temporary accumulator and will always return 1
int BoneMesh::createAnimationList(const aiNode* node, int childIdx, mat4 parent) {
    if (!scene->HasAnimations()) return 0;

    std::string nodeName(node->mName.data);
    aiMatrix4x4 nodeTrans = node->mTransformation;
    mat4 globalTrans = parent * Util::aiToGLM(&nodeTrans);
    Mesh::Animation tAnim = Mesh::Animation();
    tAnim.relTransformation = globalTrans;

    const aiAnimation* anim = scene->mAnimations[0];
    const aiNodeAnim* animNode = findNodeAnim(anim, nodeName);
    tAnim.animationLength = (float)anim->mDuration;

    // Record translation, scaling, and rotation keys
    if (animNode) {
        // translation keys
        for (unsigned int i = 0; i < animNode->mNumPositionKeys; i++) {
            const aiVectorKey& posi = animNode->mPositionKeys[i];
            const aiVector3D& v = posi.mValue;
            vec4 transKey = vec4(v.x, v.y, v.z, posi.mTime);
            tAnim.positionKeys[i] = transKey;
        }
        // scaling keys
        for (unsigned int i = 0; i < animNode->mNumScalingKeys; i++) {
            const aiVectorKey& scl = animNode->mScalingKeys[i];
            const aiVector3D& s = scl.mValue;
            vec4 scaleKey = vec4(s.x, s.y, s.z, scl.mTime);
            tAnim.scalingKeys[i] = scaleKey;
        }
        // rotation keys
        for (unsigned int i = 0; i < animNode->mNumRotationKeys; i++) {
            const aiQuatKey& rot = animNode->mRotationKeys[i];
            const aiQuaternion& q = rot.mValue;
            quat rotKey = quat(q.w, q.x, q.y, q.z);
            tAnim.rotationKeys[i] = rotKey;
            tAnim.rotationKeysTimes[i] = rot.mTime;
        }
        /* debug */
        // printf("p: %d, s: %d, r: %d\n", animNode->mNumPositionKeys, animNode->mNumScalingKeys, animNode->mNumRotationKeys);
    }

    bool isBone = boneToIndexMap.find(nodeName) != boneToIndexMap.end();  // is this node a bone in this mesh?
    if (isBone) {
        unsigned int bIndex = boneToIndexMap[nodeName];
        tAnim.globalInvTransform = Util::aiToGLM(&globalInverseTrans);
        tAnim.boneIndex = bIndex;
        animations[bIndex] = tAnim;
    }
    int tDepth = 0;
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        if (isBone) {
            unsigned nID = tDepth + childIdx + 1;
            tDepth += createAnimationList(node->mChildren[i], nID, globalTrans);
            boneInfos[tAnim.boneIndex].children[i] = nID;
        } else {
            createAnimationList(node->mChildren[i], childIdx, globalTrans);
        }
    }

    // debug
    if (isBone) {
        if (boneInfos[tAnim.boneIndex].children[0] == -1) {
            std::cout << nodeName << "(" << boneInfos[tAnim.boneIndex].ID << ")" << " has no children" << std::endl;
        } else {
            std::cout << nodeName << "(" << boneInfos[tAnim.boneIndex].ID << ")" << " has children: ";
            for (auto idx : boneInfos[tAnim.boneIndex].children) {
                if (idx == -1) break;
                std::cout << idx << ", ";
            }
            std::cout << std::endl;
        }
    }

    return 1 + tDepth;
}

std::vector<mat4> BoneMesh::getBoneTransforms(float timeSinceStarted, float animSpeed) {
    std::vector<mat4> trans(boneInfos.size(), mat4(1));
    float tps = 0.0f;
    float animTime = 0.0f;
    if (scene->HasAnimations()) {
        /* mAnimations[0] -> the first animation. change the index to access other animations */
        tps = 24 * animSpeed;                                                              // ticks per second = tps
        animTime = fmod(timeSinceStarted * tps, (float)scene->mAnimations[0]->mDuration);  // animation time in tps. fmod used for looping
    }
    trans = loadAnimation(animTime);
    return trans;
}

// Load the bone transforms of the (first) animation of this mesh at time `aTime`
// This is a proof of concept of an iteratve approach to traversing the skeleton. Later, this function will be ran in a compute shader.
// In order for that to work, it will require the entire `Animation` struct as it currently is and a reduced `BoneInfo` struct with its ID and children.
std::vector<mat4> BoneMesh::loadAnimation(float aTime) {
    std::vector<mat4> trans;
    int idStack[100];
    mat4 matStack[100];
    int idxP = 0;
    int matP = 0;
    idStack[idxP++] = 0;
    matStack[matP++] = animations[0].relTransformation;

    while (idxP > 0) {
        Mesh::Animation anima = animations[idStack[--idxP]];
        mat4 parent = matStack[--matP];

        // get new matrix from anima and aTime
        mat4 transMat = mat4(1);
        vec3 transVec = anima.interpolatePosition(aTime);
        transMat = translate(transMat, transVec);

        mat4 scaleMat = mat4(1);
        vec3 scaleVec = anima.interpolateScale(aTime);
        scaleMat = scale(scaleMat, scaleVec);

        mat4 rotMat = mat4(1);
        quat rotQuat = anima.interpolateRotation(aTime);
        rotMat = toMat4(rotQuat);

        // add matrix to trans
        auto bi = boneInfos[anima.boneIndex];
        mat4 nodeTrans = transMat * rotMat * scaleMat;
        mat4 globalTrans = parent * nodeTrans;
        mat4 finalTrans = anima.globalInvTransform * globalTrans * bi.offsetMatrix;
        bi.currentTransformation = finalTrans;
        trans.push_back(finalTrans);

        // iterative pre-order search: add to stack in reverse order
        for (int i = MAX_JOINTS_PER_BONE - 1; i >= 0; i--) {
            if (bi.children[i] != -1) {
                idStack[idxP++] = bi.children[i];
                matStack[matP++] = globalTrans;
            }
        }
    }

    return trans;
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vBones[0]) * vBones.size(), &vBones[0], GL_STATIC_DRAW);
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

std::vector<mat4> BoneMesh::getUpdatedTransforms(Shader* skinnedShader, float animSpeed) {
    return getBoneTransforms(SM::getGlobalTime(), animSpeed);
}

std::vector<mat4> BoneMesh::getUpdatedTransforms(float animSpeed) {
    return getUpdatedTransforms(shader, animSpeed);
}

void BoneMesh::update(Shader* skinnedShader, float animSpeed) {
    std::vector<mat4> trans = getUpdatedTransforms(skinnedShader, animSpeed);
    for (int i = 0; i < trans.size(); i++) {
        skinnedShader->setMat4("bones[" + std::to_string(i) + "]", trans[i]);
    }
}

void BoneMesh::update(Shader* skinnedShader) {
    update(skinnedShader, 1);
}

void BoneMesh::update(float animSpeed) {
    update(shader, animSpeed);
}

void BoneMesh::update() {
    update(1);
}
