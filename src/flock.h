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
        reset();
    }

    BoidType getTypeFromModel(std::string nm) {
        auto pth = MODEL_NO_DIR(nm);
        if (pth == "fish_threadfin") {
            return BoidType::F_THREADFIN;
        } else if (pth == "shark2") {
            return BoidType::S_BLUE;
        }
        return BoidType::S_BLUE;
    }

    // Process all boids in the flock
    void process() {
        int i = 0;
        for (auto b : boids) {
            b->process(boids);
            transforms[i] = scale(Util::lookTowards(b->pos, b->dir), BoidInfo::getBoidScale(b->type));
            i++;
        }
        vmesh->update();
    }

    void show() {
        vmesh->render(transforms.data());
    }

    void reset() {
        transforms.clear();
        boids.clear();
        int spread = 10;
        int id = 0;
        for (auto v : vmesh->variants) {
            BoidType type = getTypeFromModel(v->path);
            for (int i = 0; i < v->instanceCount; ++i) {
                Boid* boid = new Boid(
                    vec3((rand() % spread), (rand() % spread), (rand() % spread)),
                    normalize(vec3(-5 + rand() % 10, -5 + rand() % 10, -5 + rand() % 10)),
                    type,
                    id);
                boids.push_back(boid);
                transforms.push_back(translate(mat4(1), boid->pos));
                id++;
            }
        }
    }

    ~Flock() {}

    std::deque<Boid*> boids;
    std::vector<mat4> transforms;

    VariantMesh* vmesh;
};

#endif /* FLOCK_H */
