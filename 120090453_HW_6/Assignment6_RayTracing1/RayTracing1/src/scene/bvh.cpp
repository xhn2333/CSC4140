#include "bvh.h"

#include <iostream>
#include <stack>

#include "CGL/CGL.h"
#include "triangle.h"

using namespace std;

namespace CGL {
namespace SceneObjects {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {
    primitives = std::vector<Primitive *>(_primitives);
    root = construct_bvh(primitives.begin(), primitives.end(), max_leaf_size);
}

BVHAccel::~BVHAccel() {
    if (root) delete root;
    primitives.clear();
}

BBox BVHAccel::get_bbox() const { return root->bb; }

void BVHAccel::draw(BVHNode *node, const Color &c, float alpha) const {
    if (node->isLeaf()) {
        for (auto p = node->start; p != node->end; p++) {
            (*p)->draw(c, alpha);
        }
    } else {
        draw(node->l, c, alpha);
        draw(node->r, c, alpha);
    }
}

void BVHAccel::drawOutline(BVHNode *node, const Color &c, float alpha) const {
    if (node->isLeaf()) {
        for (auto p = node->start; p != node->end; p++) {
            (*p)->drawOutline(c, alpha);
        }
    } else {
        drawOutline(node->l, c, alpha);
        drawOutline(node->r, c, alpha);
    }
}

BVHNode *BVHAccel::construct_bvh(std::vector<Primitive *>::iterator start,
                                 std::vector<Primitive *>::iterator end,
                                 size_t max_leaf_size) {
    // TODO (Part 2.1):
    // Construct a BVH from the given vector of primitives and maximum leaf
    // size configuration. The starter code build a BVH aggregate with a
    // single leaf node (which is also the root) that encloses all thez
    // primitives.
    BBox bbox;
    Vector3D mean(0, 0, 0);
    int axis = 0;
    int length = end - start;

    for (auto p = start; p != end; p++) {
        BBox bb = (*p)->get_bbox();
        mean += (*p)->get_bbox().centroid();
        bbox.expand(bb);
    }

    BVHNode *node = new BVHNode(bbox);

    if (length <= max_leaf_size) {
        node->start = start;
        node->end = end;
    } else {
        auto axis = random() % 3;
        mean /= (double)length;

        double cost[3] = {0};
        for (int i = 0; i < 3; i++) {
            auto mid = std::partition(start, end, [=](const Primitive *p) {
                return p->get_bbox().centroid()[i] < mean[i];
            });
            BBox lbox, rbox;
            for (auto p = start; p != mid; p++) {
                lbox.expand((*p)->get_bbox().centroid());
            }
            for (auto p = mid; p != end; p++) {
                rbox.expand((*p)->get_bbox().centroid());
            }
            cost[i] = (mid - start) * lbox.surface_area() +
                      (end - mid) * rbox.surface_area();
        }
        axis = cost[1] < cost[2] ? (cost[0] < cost[1] ? 0 : 1)
                                 : (cost[0] < cost[2] ? 0 : 2);

        // std::cout << std::distance(start, end) << std::endl;

        auto mid = std::partition(start, end, [=](const Primitive *p) {
            return p->get_bbox().centroid()[axis] < mean[axis];
        });

        node->start = start;
        node->end = end;
        if (mid == start || mid == end) {
            node->l = nullptr;
            node->r = nullptr;
        } else {
            node->l = construct_bvh(start, mid, max_leaf_size);
            node->r = construct_bvh(mid, end, max_leaf_size);
        }
    }
    return node;
}

bool BVHAccel::has_intersection(const Ray &ray, BVHNode *node) const {
    // TODO (Part 2.3):
    // Fill in the intersect function.
    // Take note that this function has a short-circuit that the
    // Intersection version cannot, since it returns as soon as it finds
    // a hit, it doesn't actually have to find the closest hit.

    if (node->bb.intersect(ray, ray.min_t, ray.max_t)) {
        if (node->isLeaf()) {
            for (auto p = node->start; p != node->end; p++) {
                if ((*p)->has_intersection(ray)) {
                    return true;
                }
            }
        } else {
            return has_intersection(ray, node->l) ||
                   has_intersection(ray, node->r);
        }
    } else {
        return false;
    }
}

bool BVHAccel::intersect(const Ray &ray, Intersection *i, BVHNode *node) const {
    // TODO (Part 2.3):
    // Fill in the intersect function.
    /*
    bool hit = false;
    for (auto p : primitives) {
        total_isects++;
        hit = p->intersect(ray, i) || hit;
    }
    return hit;
    */
    total_isects++;
    if (node->bb.intersect(ray, ray.min_t, ray.max_t)) {
        if (node->isLeaf()) {
            bool flag = false;
            for (auto p = node->start; p != node->end; p++) {
                total_isects++;
                flag = (*p)->intersect(ray, i) || flag;
            }
            return flag;
        } else {
            bool checkl = false, checkr = false;
            if (node->l != NULL) {
                checkl = intersect(ray, i, node->l);
            }
            if (node->r != NULL) {
                checkr = intersect(ray, i, node->r);
            }
            return checkl || checkr;
        }
    } else {
        return false;
    }
}

}  // namespace SceneObjects
}  // namespace CGL
