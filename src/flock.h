#ifndef FLOCK_H
#define FLOCK_H

#include "box.h"
#include "boid.h"
#include "boidinfo.h"
#include "octree.h"
#include "variantmesh.h"

using namespace BoidInfo;

class Flock {
   public:
    Flock(VariantMesh* _vmesh) {
        vmesh = _vmesh;
        int spread = WORLD_BOUND_HIGH;
        int id = 0;
        for (auto v : vmesh->variants) {
            BoidType type = getTypeFromModel(v->path);
            for (int i = 0; i < v->instanceCount; ++i) {
                vec3 pos = vec3(-spread / 2 + (rand() % spread), -spread / 2 + (rand() % spread), -spread / 2 + (rand() % spread));
                vec3 dir = normalize(vec3(-5 + rand() % 10, -5 + rand() % 10, -5 + rand() % 10));
                Boid* boid = new Boid(
                    pos,
                    dir,
                    type,
                    id);
                boids.push_back(boid);
                BoidS bs = BoidInfo::createBoidStruct(type, id, pos, dir);
                boid_structs.push_back(bs);
                transforms.push_back(translate(mat4(1), boid->pos));
                id++;
            }
        }

        region = Box(vec3(WORLD_BOUND_LOW * 2), vec3(WORLD_BOUND_HIGH * 2));
        region.loadWireframe();
        tree = new Octree(region);
        // tree->build();
        // Util::printVec3(tree->box.size);
        // printf("%d\n", tree->mask);
        // printf("%d\n", tree->mask);

        // // create and bind ssbos to vmesh
        // boidShader = new Shader("boid shader", PROJDIR "Shaders/boids.comp");
        // glBindVertexArray(vmesh->VAO);
        // glCreateBuffers(1, &BSBO);
        // glCreateBuffers(1, &HLBO);
        // glCreateBuffers(1, &BTBO);
        // auto bufflag = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
        // glNamedBufferStorage(BSBO, boid_structs.size() * sizeof(BoidS), boid_structs.data(), bufflag);
        // glNamedBufferStorage(HLBO, cs_homes.size() * sizeof(vec4), cs_homes.data(), bufflag);
        // glNamedBufferStorage(BTBO, transforms.size() * sizeof(mat4), transforms.data(), bufflag);
        // glBindVertexArray(0);
    }

    BoidType getTypeFromModel(std::string nm) {
        auto pth = MODEL_NO_DIR(nm);
        if (pth == "fish_threadfin") {
            return BoidType::F_THREADFIN;
        } else if (pth == "fish_marlin") {
            return BoidType::F_MARLIN;
        } else if (pth == "fish_spear") {
            return BoidType::F_SPEAR_FISH;
        } else if (pth == "shark2") {
            return BoidType::S_BLUE;
        }
        return BoidType::S_BLUE;
    }

    // Process all boids in the flock
    void process() {
        std::vector<vec3> homes = {vec3(0, 10, 0), vec3(0, -10, 0), vec3(10)};
        // for (int i = 0; i < transforms.size(); ++i) {
        //     Boid* b = boids[i];
        //     b->process(boids, homes);
        //     transforms[i] = scale(Util::lookTowards(b->pos, b->dir), BoidInfo::getBoidScale(b->type));
        // }
        // todo
        // tree->update();
        // printf("%d\n", bs.size());
        // printf("%d\n", tree->mask);
        // tree = new Octree(boids, region, true);
        // tree->build();
        // int pp = 0;
        // auto tbs = tree->getAllBoids();
        // for (const auto& p : tbs) {
        //     // Util::printList(p);
        //     pp += p.size();
        // }
        // printf("%d, %d\n\n", pp, tbs.size());
        // assert(pp == boids.size());
        // int i = 0;
        // tree->update();
        region.drawWireframe();
        // if (tree->regenerateFlag) {
        //     printf("update failure. failed to insert: %s\n", tree->regenerateFlag ? "true" : "false");
        // }
        // if (!tree->update()) {
        //     printf("failed to update. failed to insert: %s\n", tree->regenerateFlag ? "true" : "false");
        // }
        
        // delete tree;
        // tree = new Octree(region);
        tree->reset();
        // tree->build();
        for (const auto& b : boids) {
            tree->insert(b);
            // printf("here\n");
        }
        tree->updateTransforms({transforms, homes});
        // auto bs = tree->getBoidsList();
        // printf("[");
        // for (const auto& b : bs) {
        //     printf("%d, ", b->ID);
        // }
        // printf("]\n");

        // glBindVertexArray(vmesh->VAO);
        // boidShader->use();
        // boidShader->setFloat("deltaTime", SM::delta);
        // boidShader->setBool("canAttack", true);
        // boidShader->setVec3("gridSize", vec3(WORLD_BOUND_HIGH));
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, BSBO);
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, HLBO);
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, BTBO);
        // glDispatchCompute(std::max(transforms.size()/1024, 1ULL), 1, 1);                                // declare work group sizes and run compute shader
        // glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);               // wait for all threads to be finished
        // glBindVertexArray(0);

        // BoidS *matptr;
        // matptr = (BoidS *)glMapNamedBuffer(BSBO, GL_READ_WRITE);
        // bool printed = false;
        // for (int i = 0; i < transforms.size(); ++i) {
        //     // Util::printMat4(matptr[i]);
        //     // Util::printVec3(matptr[i].pos);
        //     BoidS b = matptr[i];
        //     // if (b.isBeingChased || b.isChasing) {
        //     //     printf("ID: %d, type: %d, fleeing: %d, chasing: %d, around: %d\n", b.ID, b.type, b.isBeingChased, b.isChasing, b.boidsAround);
        //     //     printed = true;
        //     // }
        //     if (b.type == BoidType::S_BLUE && b.boidsAround != 0) {
        //         printf("around: %.2f\n", b.boidsAround);
        //         printed = true;
        //     }
        // }
        // // if (printed) printf("\n");
        // glUnmapNamedBuffer(BSBO);

        // mat4 *matptr;
        // matptr = (mat4 *)glMapNamedBuffer(BTBO, GL_READ_WRITE);
        // for (int i = 0; i < transforms.size(); ++i) {
        //     // // Util::printMat4(matptr[i]);
        //     // Util::printVec3(matptr[i].pos);
        //     transforms[i] = matptr[i];
        // }
        // // printf("\n");
        // glUnmapNamedBuffer(BTBO);
        vmesh->update();
    }

    void show() {
        vmesh->render(transforms.data());
    }

    void reset() {
        int spread = WORLD_BOUND_HIGH;
        for (int i = 0; i < boids.size(); ++i) {
            vec3 np = vec3(-spread / 2 + (rand() % spread), -spread / 2 + (rand() % spread), -spread / 2 + (rand() % spread));
            vec3 nv = normalize(vec3(-5 + rand() % 10, -5 + rand() % 10, -5 + rand() % 10));
            Boid* b = boids[i];
            b->pos = np;
            b->resetVelocity();
            boid_structs[i].pos = vec4(np, 1);
            boid_structs[i].velocity = vec4(nv, 1);
        }
        tree->reset();
    }

    ~Flock() {}

    std::vector<Boid*> boids;
    Box region;
    Octree* tree;
    std::vector<BoidS> boid_structs;
    std::vector<mat4> transforms;
    VariantMesh* vmesh;
    Shader* boidShader;
    std::vector<vec4> cs_homes = {vec4(0, 50, 0, 1), vec4(0, -50, 0, 1), vec4(50)};

    unsigned int BSBO;  // boid structs
    unsigned int HLBO;  // home locations
    unsigned int BTBO;  // boid transforms
};

#endif /* FLOCK_H */
