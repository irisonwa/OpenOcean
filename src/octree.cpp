#include "octree.h"

void Octree::updateTransforms(std::pair<std::vector<mat4>&, std::vector<vec3>> transformsAndHomes) {
    // ! VERY SLOW! do not enable for more than 64 boids
    // box.drawWireframe();

    auto [transforms, homes] = transformsAndHomes;
    for (Boid* b : boids) {
        b->process(boids, homes);
        transforms[b->ID] = scale(Util::lookTowards(b->pos, b->dir), BoidInfo::getBoidScale(b->type));
    }
    for (auto child : children) {
        if (child) {
            child->updateTransforms(transformsAndHomes);
        }
    }
}

void Octree::insert(Boid* boid) {
    // if the box is at or below the minimum size, add it
    auto bsize = box.size;
    if (bsize.x <= MIN_BOX_SIZE && bsize.y <= MIN_BOX_SIZE && bsize.z <= MIN_BOX_SIZE) {
        boids.push_back(boid);
        return;
    }

    // if the box can fit it, add it
    if (boids.size() < MAX_CHILD_COUNT) {
        boids.push_back(boid);
        return;
    }

    int octant = box.getOctant(boid->pos);
    auto nBoxs = box.split();
    Box nBox = nBoxs[octant];

    if (!children[octant]) children[octant] = new Octree(nBox);
    children[octant]->insert(boid);
}

void Octree::reset() {
    for (int i = 0; i < OCT; ++i) {
        if (children[i]) children[i]->reset();
        delete children[i];
        children[i] = nullptr;
    }
    boids.clear();
}
