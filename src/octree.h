#ifndef OCTREE_H
#define OCTREE_H

#define OCT 8
#define MIN_BOX_SIZE 2
#define MAX_CHILD_COUNT 32

#include "boid.h"
#include "box.h"
#include "sm.h"
#include "util.h"

class Octree {
   public:
    Octree() {}
    Octree(Box bound) { 
        box = bound;
        for (int i = 0; i < OCT; ++i) children.push_back(nullptr);
    }
    Octree(std::vector<Boid *> &bs, Box bound) {
        boids = bs;
        box = bound;
        for (int i = 0; i < OCT; ++i) children.push_back(nullptr);
    }
    ~Octree() {
        for (int i = 0; i < OCT; ++i) {
            delete children[i];
        }
        boids.clear();
    }

    void build();
    Octree *createChild(std::vector<Boid *>&, Box);
    std::vector<Boid*> getBoidsList(); // get all child boids as a 1d list
    std::vector<std::vector<Boid*>> getAllBoids(); // get all child boids as a 1d list
    std::vector<std::vector<Boid*>> getAllBoids(std::vector<std::vector<Boid*>>, std::vector<Boid*>);
    bool update();
    bool isLeaf();
    void updateTransforms(std::pair<std::vector<mat4>&, std::vector<vec3>> transformsAndHomes);
    void insert(Boid*);
    void reset();

    std::vector<Octree*> children;
    std::vector<Boid *> boids;
    Box box;
    bool odebug = false;
};

#endif /* OCTREE_H */
