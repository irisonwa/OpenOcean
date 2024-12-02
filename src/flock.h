#ifndef FLOCK_H
#define FLOCK_H

// #define TREE
#define DISPATCH_SIZE 1024

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
        int spread = WORLD_BOUND_HIGH/4;
        int id = 0;
        bc = new BoidContainer();
        bc->boids = new Boid*[vmesh->totalInstanceCount];
        bc->size = vmesh->totalInstanceCount;
        for (auto v : vmesh->variants) {
            BoidType type = getTypeFromModel(v->path);
            for (int i = 0; i < v->instanceCount; ++i) {
                vec3 pos = Util::randomv(-spread / 2, spread / 2);
                vec3 vel = Util::randomv(-5, 5);
                Boid* boid = new Boid(
                    pos,
                    vel,
                    type,
                    id);
                bc->boids[id] = boid;
                BoidS bs = BoidInfo::createBoidStruct(type, id, pos, vel);
                boid_structs.push_back(bs);
                transforms.push_back(translate(mat4(1), pos));
                id++;
            }
        }

        region = Box(vec3(WORLD_BOUND_LOW * 2), vec3(WORLD_BOUND_HIGH * 2));
        region.loadWireframe();
#ifdef TREE
        tree = new Octree(bc, region);
#else
        // todo: somehow the direction always includes null values, which are annoying to deal with in the compute shader
        // create and bind ssbos to vmesh
        boidShader = new Shader("boid shader", PROJDIR "Shaders/boids.comp");
        glBindVertexArray(vmesh->VAO);
        glCreateBuffers(1, &BSBO);
        glCreateBuffers(1, &HLBO);
        glCreateBuffers(1, &BTBO);
        auto bufflag = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
        glNamedBufferStorage(BSBO, boid_structs.size() * sizeof(BoidS), boid_structs.data(), bufflag);
        glNamedBufferStorage(HLBO, cs_homes.size() * sizeof(vec4), cs_homes.data(), bufflag);
        glNamedBufferStorage(BTBO, transforms.size() * sizeof(mat4), transforms.data(), bufflag);
        glBindVertexArray(0);
#endif
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
        region.drawWireframe();
#ifdef TREE
        tree->reset();
        for (int i = 0; i < bc->size; ++i) {
            tree->insert(bc->boids[i]);
            // printf("here\n");
        }
        for (int i = 0; i < bc->size; ++i) {
            flockBoid(bc->boids[i]);
            transforms[i] = scale(Util::lookTowards(bc->boids[i]->pos, bc->boids[i]->dir), BoidInfo::getBoidScale(bc->boids[i]->type));
        }
#else
        glBindVertexArray(vmesh->VAO);
        boidShader->use();
        boidShader->setFloat("deltaTime", SM::delta);
        boidShader->setBool("canAttack", true);
        boidShader->setVec3("gridSize", vec3(WORLD_BOUND_HIGH));
        // boidShader->setInt("idBase", 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, BSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, HLBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, BTBO);

        // Dispatch loop
        // In the compute shader, each thread only affects the value at its invocation index + idBase
        int n = transforms.size();
        int idBase = 0;
        while (n > 0) {
            boidShader->setInt("idBase", idBase);
            glDispatchCompute(DISPATCH_SIZE, 1, 1);                     // declare work group sizes and run compute shader
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);  // wait for all threads to be finished
            n -= DISPATCH_SIZE;
            idBase += DISPATCH_SIZE;
        }
        // glDispatchCompute(std::max(transforms.size()/1024, 1ULL), 1, 1);                                // declare work group sizes and run compute shader
        // // glDispatchCompute(1, 1, 1);                                // declare work group sizes and run compute shader
        // glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);               // wait for all threads to be finished

        // BoidS *matptr;
        // matptr = (BoidS *)glMapNamedBuffer(BSBO, GL_READ_WRITE);
        // if (matptr) {
        //     bool printed = false;
        //     for (int i = 0; i < transforms.size(); ++i) {
        //         // Util::printMat4(matptr[i]);
        //         // Util::printVec3(matptr[i].pos);
        //         BoidS b = matptr[i];
        //         // if (b.isBeingChased || b.isChasing) {
        //         //     printf("ID: %d, type: %d, fleeing: %d, chasing: %d, around: %d\n", b.ID, b.type, b.isBeingChased, b.isChasing, b.boidsAround);
        //         //     printed = true;
        //         // }
        //         // Util::printVec3(b.dir);
        //         // if (b.type == BoidType::S_BLUE && b.boidsAround != 0) {
        //             printf("around: %.2f\n", b.boidsAround);
        //             // printf("around: %d\n", b.boidsAround);
        //         //     printed = true;
        //         // }
        //     }
        // } else {
        //     printf("map failed\n");
        // }
        // // if (printed) printf("\n");
        // glUnmapNamedBuffer(BSBO);
        // glBindVertexArray(0);

        // mat4 *matptr;
        // matptr = (mat4 *)glMapNamedBuffer(BTBO, GL_READ_WRITE);
        // for (int i = 0; i < transforms.size(); ++i) {
        //     // // Util::printMat4(matptr[i]);
        //     // Util::printVec3(matptr[i].pos);
        //     transforms[i] = matptr[i];
        // }
        // // printf("\n");
        // glUnmapNamedBuffer(BTBO);
#endif
    }

    void flockBoid(Boid* b) {
        int cnt = 0;
        const unsigned* bs = tree->getBoidsInRange(b->pos, b->visibleRange, cnt);
        b->process(bc, bs, cnt, homes);
    }

    void show() {
        vmesh->render(transforms.data());
    }

    void reset() {
        int spread = WORLD_BOUND_HIGH;
        for (int i = 0; i < bc->size; ++i) {
            vec3 np = vec3(-spread / 2 + (rand() % spread), -spread / 2 + (rand() % spread), -spread / 2 + (rand() % spread));
            vec3 nv = normalize(vec3(-5 + rand() % 10, -5 + rand() % 10, -5 + rand() % 10));
            Boid* b = bc->boids[i];
            b->pos = np;
            b->resetVelocity();
            boid_structs[i].pos = vec4(np, 1);
            boid_structs[i].velocity = vec4(nv, 1);
        }
        tree->reset();
    }

    ~Flock() {}

    BoidContainer* bc;
    Box region;
    Octree* tree;
    std::vector<BoidS> boid_structs;
    std::vector<mat4> transforms;
    VariantMesh* vmesh;
    Shader* boidShader;
    std::vector<vec4> cs_homes = {vec4(0, 50, 0, 1), vec4(0, -50, 0, 1), vec4(50)};
    std::vector<vec3> homes = {vec3(0, 10, 0), vec3(0, -10, 0), vec3(10)};

    unsigned int BSBO;  // boid structs
    unsigned int HLBO;  // home locations
    unsigned int BTBO;  // boid transforms
};

#endif /* FLOCK_H */
