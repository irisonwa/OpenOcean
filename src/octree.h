#ifndef OCTREE_H
#define OCTREE_H

#define OCT 8
#define MIN_BOX_SIZE 1
#define MAX_CHILD_COUNT 64
#define MAX_CHECK 1024

#include "boid.h"
#include "box.h"
#include "sm.h"
#include "util.h"

class Octree {
   public:
    Octree() {}
    Octree(Box bound) { 
        box = bound;
        for (int i = 0; i < OCT; ++i) children[i] = nullptr;
    }

    Octree(BoidContainer* &cnt, Box bound) : bc(cnt) {
        box = bound;
        for (int i = 0; i < OCT; ++i) children[i] = nullptr;
    }

    ~Octree() {
        for (int i = 0; i < OCT; ++i) {
            delete children[i];
            children[i] = nullptr;
        }
    }

    // void build();
    // Octree *createChild(std::vector<Boid *>&, Box);
    // std::vector<Boid*> getBoidsList(); // get all child boids as a 1d list
    // std::vector<std::vector<Boid*>> getAllBoids(); // get all child boids as a 1d list
    // std::vector<std::vector<Boid*>> getAllBoids(std::vector<std::vector<Boid*>>, std::vector<Boid*>);
    // bool update();
    // bool isLeaf();
    // void updateTransforms(std::pair<std::vector<mat4>&, std::vector<vec3>> transformsAndHomes);

    const unsigned* getBoidsInRange(vec3 origin, float range, int &count);
    unsigned* getBoidsInRange(vec3 origin, float range, int &count, unsigned *acc);
    void insert(Boid*);
    void reset();

    Octree* children[OCT];
    BoidContainer* bc;
    unsigned indices[MAX_CHILD_COUNT * MAX_CHILD_COUNT];  // list of boid indices
    unsigned bcount = 0; // number of boids in the list. always points to the top of the `indices` list.
    Box box;
};

#endif /* OCTREE_H */
