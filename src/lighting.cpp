#include "lighting.h"

void Lighting::use() {
    switch (material) {
        case MATERIAL_STONE:
            loadStoneMaterial();
            break;
        case MATERIAL_PLASTIC:
            loadPlasticMaterial();
            break;
        case MATERIAL_WOOD:
            loadWoodMaterial();
            break;
        case MATERIAL_RUBBER:
            loadRubberMaterial();
            break;
        case MATERIAL_METAL:
            loadMetalMaterial();
            break;
        case MATERIAL_SHINY:
            loadShinyMaterial();
            break;
        default:
            printf("invalid material for light");
            break;
    }
}

void Lighting::loadMaterial() {
    shader->setInt("nDirLights", nDirLights);
    shader->setInt("nSpotLights", nSpotLights);
    shader->setInt("nPointLights", nPointLights);

    for (int i = 0; i < nDirLights; i++) {
        shader->setVec3("dirLights[" + std::to_string(i) + "].direction", dirLights[i].direction);
        shader->setVec3("dirLights[" + std::to_string(i) + "].ambient", dirLights[i].ambient);
        shader->setVec3("dirLights[" + std::to_string(i) + "].diffuse", dirLights[i].diffuse);
        shader->setVec3("dirLights[" + std::to_string(i) + "].specular", dirLights[i].specular);
    }

    for (int i = 0; i < nPointLights; i++) {
        shader->setVec3("pointLights[" + std::to_string(i) + "].position", pointLights[i].position);
        shader->setVec3("pointLights[" + std::to_string(i) + "].ambient", pointLights[i].ambient);
        shader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLights[i].diffuse);
        shader->setVec3("pointLights[" + std::to_string(i) + "].specular", pointLights[i].specular);
        shader->setFloat("pointLights[" + std::to_string(i) + "].constant", pointLights[i].constant);
        shader->setFloat("pointLights[" + std::to_string(i) + "].linear", pointLights[i].linear);
        shader->setFloat("pointLights[" + std::to_string(i) + "].quadratic", pointLights[i].quadratic);
    }

    for (int i = 0; i < nSpotLights; i++) {
        shader->setVec3("spotLights[" + std::to_string(i) + "].position", spotLights[i].position);
        shader->setVec3("spotLights[" + std::to_string(i) + "].direction", spotLights[i].direction);
        shader->setVec3("spotLights[" + std::to_string(i) + "].ambient", spotLights[i].ambient);
        shader->setVec3("spotLights[" + std::to_string(i) + "].diffuse", spotLights[i].diffuse);
        shader->setVec3("spotLights[" + std::to_string(i) + "].specular", spotLights[i].specular);
        shader->setFloat("spotLights[" + std::to_string(i) + "].constant", spotLights[i].constant);
        shader->setFloat("spotLights[" + std::to_string(i) + "].linear", spotLights[i].linear);
        shader->setFloat("spotLights[" + std::to_string(i) + "].quadratic", spotLights[i].quadratic);
        shader->setFloat("spotLights[" + std::to_string(i) + "].cutOff", spotLights[i].cutOff);
        shader->setFloat("spotLights[" + std::to_string(i) + "].outerCutOff", spotLights[i].outerCutOff);
    }
}

void Lighting::loadStoneMaterial() {
    shader->use();
    shader->setFloat("material.shininess", 5.f);
    shader->setInt("material.diffuse", 0);
    shader->setInt("material.specular", 1);
    loadMaterial();
}

void Lighting::loadMetalMaterial() {
    shader->use();
    shader->setFloat("material.shininess", 100.0f);
    shader->setInt("material.diffuse", 0);
    shader->setInt("material.specular", 1);
    loadMaterial();
}

void Lighting::loadWoodMaterial() {
    shader->use();
    shader->setFloat("material.shininess", 20.0f);
    shader->setInt("material.diffuse", 0);
    shader->setInt("material.specular", 1);
    loadMaterial();
}

void Lighting::loadRubberMaterial() {
    shader->use();
    shader->setFloat("material.shininess", 0.1f);
    shader->setInt("material.diffuse", 0);
    shader->setInt("material.specular", 1);
    loadMaterial();
}

void Lighting::loadPlasticMaterial() {
    shader->use();
    shader->setFloat("material.shininess", 50.0f);
    shader->setInt("material.diffuse", 0);
    shader->setInt("material.specular", 1);
    loadMaterial();
}

void Lighting::loadShinyMaterial() {
    shader->use();
    shader->setFloat("material.shininess", 850.0f);
    shader->setInt("material.diffuse", 0);
    shader->setInt("material.specular", 1);
    loadMaterial();
}

void Lighting::setLightAtt(mat4 view, mat4 projection, vec3 vPos) {
    shader->use();
    shader->setVec3("viewPos", vPos);
    shader->setMat4("view", view);
    shader->setMat4("proj", projection);
}

void Lighting::setSpotLightsAtt(std::vector<vec3> pos, std::vector<vec3> dir) {
    nSpotLights = pos.size();
    for (int i = 0; i < pos.size(); i++) {
        spotLights[i].position = pos[i];
        spotLights[i].direction = dir[i];
        spotLights[i].constant = 1.f;
        spotLights[i].linear = 0.09f;
        spotLights[i].quadratic = 0.032f;
        spotLights[i].cutOff = cos(Util::deg2Rad(12.5f));
        spotLights[i].outerCutOff = cos(Util::deg2Rad(15.5f));
    }
}

void Lighting::addSpotLightsAtt(std::vector<vec3> pos, std::vector<vec3> dir) {
    for (int i = 0; i < Util::clamp(nSpotLights + pos.size(), 0, 100); i++) {
        spotLights[i + nSpotLights].position = pos[i];
        spotLights[i + nSpotLights].direction = dir[i];
        spotLights[i].constant = 1.f;
        spotLights[i].linear = 0.09f;
        spotLights[i].quadratic = 0.032f;
        spotLights[i].cutOff = cos(Util::deg2Rad(12.5f));
        spotLights[i].outerCutOff = cos(Util::deg2Rad(15.5f));
        nSpotLights++;
    }
}

void Lighting::addSpotLightAtt(vec3 pos, vec3 dir, vec3 amb, vec3 dif, vec3 spec) {
    if (nSpotLights < 100) {
        spotLights[nSpotLights].position = pos;
        spotLights[nSpotLights].direction = dir;
        spotLights[nSpotLights].ambient = amb;
        spotLights[nSpotLights].diffuse = dif;
        spotLights[nSpotLights].specular = spec;
        spotLights[nSpotLights].constant = 1.f;
        spotLights[nSpotLights].linear = 0.09f;
        spotLights[nSpotLights].quadratic = 0.032f;
        spotLights[nSpotLights].cutOff = cos(Util::deg2Rad(12.5f));
        spotLights[nSpotLights].outerCutOff = cos(Util::deg2Rad(15.5f));
        nSpotLights++;
    }
}

void Lighting::setSpotLightAtt(int idx, vec3 pos, vec3 dir, vec3 amb, vec3 dif, vec3 spec) {
    if (idx < 100) {
        spotLights[idx].position = pos;
        spotLights[idx].direction = dir;
        spotLights[idx].ambient = amb;
        spotLights[idx].diffuse = dif;
        spotLights[idx].specular = spec;
    }
}

void Lighting::setPointLightsAtt(std::vector<vec3> pos) {
    nPointLights = pos.size();
    for (int i = 0; i < pos.size(); i++) {
        pointLights[i].position = pos[i];
        pointLights[i].constant = 1.f;
        pointLights[i].linear = 0.09f;
        pointLights[i].quadratic = 0.032f;
    }
}

void Lighting::addPointLightAtt(vec3 pos, vec3 amb, vec3 dif, vec3 spec) {
    if (nPointLights < 100) {
        pointLights[nPointLights].position = pos;
        pointLights[nPointLights].ambient = amb;
        pointLights[nPointLights].diffuse = dif;
        pointLights[nPointLights].specular = spec;
        pointLights[nPointLights].constant = 1.f;
        pointLights[nPointLights].linear = 0.09f;
        pointLights[nPointLights].quadratic = 0.032f;
        nPointLights++;
    }
}

void Lighting::setDirLightsAtt(std::vector<vec3> dir) {
    nDirLights = dir.size();
    for (int i = 0; i < dir.size(); i++) {
        dirLights[i].direction = dir[i];
    }
}

void Lighting::setSpotLightColours(std::vector<vec3> amb, std::vector<vec3> dif, std::vector<vec3> spe) {
    for (int i = 0; i < amb.size(); i++) {
        spotLights[i].ambient = amb[i];
    }

    for (int i = 0; i < dif.size(); i++) {
        spotLights[i].diffuse = dif[i];
    }

    for (int i = 0; i < spe.size(); i++) {
        spotLights[i].specular = spe[i];
    }
}

void Lighting::setPointLightColours(std::vector<vec3> amb, std::vector<vec3> dif, std::vector<vec3> spe) {
    for (int i = 0; i < amb.size(); i++) {
        pointLights[i].ambient = amb[i];
    }

    for (int i = 0; i < dif.size(); i++) {
        pointLights[i].diffuse = dif[i];
    }

    for (int i = 0; i < spe.size(); i++) {
        pointLights[i].specular = spe[i];
    }
}

void Lighting::setDirLightColours(std::vector<vec3> amb, std::vector<vec3> dif, std::vector<vec3> spe) {
    nDirLights = 1;
    for (int i = 0; i < amb.size(); i++) {
        dirLights[i].ambient = amb[i];
    }

    for (int i = 0; i < dif.size(); i++) {
        dirLights[i].diffuse = dif[i];
    }

    for (int i = 0; i < spe.size(); i++) {
        dirLights[i].specular = spe[i];
    }
}

void Lighting::setDirLightColour(vec3 amb, vec3 dif, vec3 spe) {
    nDirLights = 1;
    dirLights[0].ambient = amb;
    dirLights[0].diffuse = dif;
    dirLights[0].specular = spe;
}