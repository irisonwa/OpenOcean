#ifndef FLOCK_H
#define FLOCK_H

#include "boid.h"
#include "variantmesh.h"
#include "boidinfo.h"

using namespace BoidInfo;

class Flock {
   private:
    /* data */
   public:
    Flock(VariantMesh* _vmesh) {
        vmesh = _vmesh;
        int spread = 10;
        int id = 0;
        for (auto v : vmesh->variants) {
            BoidType type = getTypeFromModel(v->path);
            for (int i = 0; i < v->instanceCount; ++i) {
                vec3 pos = vec3((rand() % spread), 10, (rand() % spread));
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
        int i = 0;
        std::vector<vec3> homes = {vec3(0, 50, 0), vec3(0, -50, 0), vec3(50)};
        for (auto b : boids) {
            b->process(boids, homes);
            transforms[i] = scale(Util::lookTowards(b->pos, b->dir), BoidInfo::getBoidScale(b->type));
            i++;
        }
        vmesh->update();
    }

    void show() {
        vmesh->render(transforms.data());
    }

    void reset() {
        int spread = 10;
        for (auto b : boids) {
            b->pos = vec3((rand() % spread), 10, (rand() % spread));
            b->velocity = normalize(vec3(-5 + rand() % 10, -5 + rand() % 10, -5 + rand() % 10));
        }
    }

    ~Flock() {}

    std::deque<Boid*> boids;
    std::deque<BoidS> boid_structs;
    std::vector<mat4> transforms;
    VariantMesh* vmesh;
    Shader *boidShader;
};

#endif /* FLOCK_H */
