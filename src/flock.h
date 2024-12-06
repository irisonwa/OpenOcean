#ifndef FLOCK_H
#define FLOCK_H

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
        int spread = WORLD_BOUND_HIGH;
        int id = 0;
        bc = new BoidContainer();
        bc->boids = new Boid*[vmesh->totalInstanceCount];
        boid_count = bc->size = vmesh->totalInstanceCount;
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

#ifdef TREE
        region = Box(vec3(WORLD_BOUND_LOW * 2), vec3(WORLD_BOUND_HIGH * 2));
        region.loadWireframe();
        tree = new Octree(bc, region);
#else
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
    process();
    }

    BoidType getTypeFromModel(std::string nm) {
        auto pth = MODEL_NO_DIR(nm);
        if (pth == "fish_threadfin") {
            return BoidType::F_THREADFIN;
        } else if (pth == "fish_marlin") {
            return BoidType::F_MARLIN;
        } else if (pth == "fish_spear") {
            return BoidType::F_SPEAR_FISH;
        } else if (pth == "fish_clown") {
            return BoidType::F_CLOWNFISH;
        } else if (pth == "fish_herring") {
            return BoidType::F_HERRING;
        } else if (pth == "shark_whale") {
            return BoidType::S_WHALE;
        } else if (pth == "shark_white") {
            return BoidType::S_WHITE;
        } else if (pth == "shark_blue") {
            return BoidType::S_BLUE;
        } else if (pth == "whale") {
            return BoidType::WHALE;
        } else if (pth == "dolphin") {
            return BoidType::DOLPHIN;
        } else if (pth == "plankton") {
            return BoidType::PLANKTON;
        }
        return BoidType::F_THREADFIN;
    }

    // Process all boids in the flock
    void process() {
#ifdef TREE
        region.drawWireframe();
        tree->reset();
        for (int i = 0; i < bc->size; ++i) {
            tree->insert(bc->boids[i]);
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
#ifdef TREE
        for (int i = 0; i < boid_count; ++i) {
            vec3 np = Util::randomv(-spread / 2, spread / 2);
            vec3 nv = normalize(Util::randomv(-5, 5));
            Boid* b = bc->boids[i];
            b->pos = np;
            b->resetVelocity();
        }
        tree->reset();
#else
        BoidS *map_bs;
        map_bs = (BoidS *)glMapNamedBuffer(BSBO, GL_READ_WRITE);
        if (map_bs) {
            for (int i = 0; i < boid_count; ++i) {
                vec3 np = Util::randomv(-spread / 2, spread / 2);
                vec3 nv = normalize(Util::randomv(-5, 5));
                BoidS b = map_bs[i];
                b.pos = vec4(np, 0);
                b.velocity = vec4(nv, 0);
            }

        } else {
            printf("map failed\n");
        }
        // if (printed) printf("\n");
        glUnmapNamedBuffer(BSBO);
#endif
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
    int boid_count = 0;

    unsigned int BSBO;  // boid structs
    unsigned int HLBO;  // home locations
    unsigned int BTBO;  // boid transforms
};

#endif /* FLOCK_H */
