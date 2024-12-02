#include "octree.h"

// void Octree::updateTransforms(std::pair<std::vector<mat4>&, std::vector<vec3>> transformsAndHomes) {
//     // ! VERY SLOW! do not enable for more than 64 boids
//     // box.drawWireframe();

//     auto [transforms, homes] = transformsAndHomes;
//     for (Boid* b : boids) {
//         b->process(boids, homes);
//         transforms[b->ID] = scale(Util::lookTowards(b->pos, b->dir), BoidInfo::getBoidScale(b->type));
//     }
//     for (auto child : children) {
//         if (child) {
//             child->updateTransforms(transformsAndHomes);
//         }
//     }
// }

void Octree::insert(Boid* boid) {
    // if the box is at or below the minimum size, add it (last resort)
    // printf("h0\n");
    auto bsize = box.size;
    if (bsize.x <= MIN_BOX_SIZE && bsize.y <= MIN_BOX_SIZE && bsize.z <= MIN_BOX_SIZE) {
        // printf("h0.5 - %d\n", bcount);
        indices[bcount++] = boid->ID;
        return;
    }

    // if the box can fit it, add it
    if (bcount < MAX_CHILD_COUNT) {
        // printf("h1\n");
        indices[bcount++] = boid->ID;
        // printf("h2\n");
        return;
    }

    int octant = box.getOctant(boid->pos);
    auto nBoxs = box.split();
    Box nBox = nBoxs[octant];

    // printf("h3\n");
    if (!children[octant]) {
        children[octant] = new Octree(nBox);
        children[octant]->bc = bc;
        // printf("h4\n");
    }
    children[octant]->insert(boid);
    // printf("h5\n");
}

// Get a list of boid indices within the radius `range` around `origin`. The size of the list will be held in `count`.
const unsigned* Octree::getBoidsInRange(vec3 origin, float range, int &count) {
    unsigned *idxs = new unsigned[MAX_CHECK];
    int cnt = 0;
    getBoidsInRange(origin, range, cnt, idxs);
    count = cnt;
    return idxs;
}

// helper function
unsigned* Octree::getBoidsInRange(vec3 origin, float range, int &count, unsigned* acc) {
    float dist = range * range;
    // for (int i = 0; i < OCT; ++i) {
    //     if (count >= MAX_CHECK) break;
    //     // printf("here 3.1.2\n");
    //     if (children[i] && children[i]->box.inRange(origin, dist)) {
    //         // printf("here 3.1.3 - %d, %d\n", count, bcount);
    //         // printf("here 3.1.4 - %d\n", count);
    //         acc = children[i]->getBoidsInRange(origin, range, count, acc);
    //     }
    // }
    int octant = box.getOctant(origin);
    // assert(children[octant]);
    if (children[octant]) acc = children[octant]->getBoidsInRange(origin, range, count, acc);

    for (int i = 0; i < bcount && count < MAX_CHECK; ++i) {
        auto idx = indices[i];
        if (idx != -1 && Util::sqDist(bc->boids[idx]->pos, origin) <= dist) {
            // printf("here 3.1.1.0.1 - %d\n", *count);
            acc[count++] = idx;
            // printf("here 3.1.1.0.2 - %d, %d, %d\n", i, count, bcount);
        }
    }
    // printf("here 3.1.1.1 - %d\n", *count);
    return acc;
}

void Octree::reset() {
    // for (int i = 0; i < MAX_CHILD_COUNT; ++i) indices[i] = -1;
    bcount = 0;
    for (int i = 0; i < OCT; ++i) {
        if (children[i]) {
            children[i]->reset();
            // delete children[i];
            // children[i] = nullptr;
        }
    }
}
