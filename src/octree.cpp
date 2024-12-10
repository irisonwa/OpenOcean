#include "octree.h"

void Octree::insert(Boid* boid) {
    // if the box is at or below the minimum size, add it (last resort)
    auto bsize = box.size;
    if (bsize.x <= MIN_BOX_SIZE && bsize.y <= MIN_BOX_SIZE && bsize.z <= MIN_BOX_SIZE) {
        indices[bcount++] = boid->ID;
        return;
    }

    // if the box can fit it, add it
    if (bcount < MAX_CHILD_COUNT) {
        indices[bcount++] = boid->ID;
        return;
    }

    int octant = box.getOctant(boid->pos);
    auto nBoxs = box.split();
    Box nBox = nBoxs[octant];

    if (!children[octant]) {
        children[octant] = new Octree(nBox);
        children[octant]->bc = bc;
    }
    children[octant]->insert(boid);
}

// Get a list of boid indices within the radius `range` around `origin`. The size of the list will be held in `count`.
const unsigned* Octree::getBoidsInRange(vec3 origin, float range, int& count) {
    unsigned* idxs = new unsigned[MAX_CHECK];
    int cnt = 0;
    getBoidsInRange(origin, range, cnt, idxs);
    count = cnt;
    return idxs;
}

// helper function
unsigned* Octree::getBoidsInRange(vec3 origin, float range, int& count, unsigned* acc) {
    float dist = range * range;
    int octant = box.getOctant(origin);
    if (children[octant]) acc = children[octant]->getBoidsInRange(origin, range, count, acc);

    for (int i = 0; i < bcount && count < MAX_CHECK; ++i) {
        auto idx = indices[i];
        if (idx != -1 && Util::sqDist(bc->boids[idx]->pos, origin) <= dist) {
            acc[count++] = idx;
        }
    }
    return acc;
}

void Octree::reset() {
    bcount = 0;
    for (int i = 0; i < OCT; ++i) {
        if (children[i]) {
            children[i]->reset();
            // delete children[i];
            // children[i] = nullptr;
        }
    }
}
