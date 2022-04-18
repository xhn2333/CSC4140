#include "sphere.h"

#include <cmath>

#include "pathtracer/bsdf.h"
#include "util/sphere_drawing.h"

namespace CGL {
namespace SceneObjects {

bool Sphere::test(const Ray &r, double &t1, double &t2) const {
    // TODO (Part 1.4):
    // Implement ray - sphere intersection test.
    // Return true if there are intersections and writing the
    // smaller of the two intersection times in t1 and the larger in t2.
    double a = (r.d).norm2();
    double b = 2 * dot((r.o - o), r.d);
    double c = (r.o - o).norm2() - r2;

    double test = b * b - 4 * a * c;
    t1 = r.min_t - 1;
    t2 = r.max_t + 1;

    if (test >= 0) {
        t2 = (-b + sqrt(test)) / (2 * a);
        t1 = (-b - sqrt(test)) / (2 * a);
    }

    return ((t1 >= r.min_t) && (t1 <= r.min_t)) ||
           ((t2 >= r.min_t) && (t2 <= r.min_t));
}

bool Sphere::has_intersection(const Ray &r) const {
    // TODO (Part 1.4):
    // Implement ray - sphere intersection.
    // Note that you might want to use the the Sphere::test helper here.
    double a = (r.d).norm2();
    double b = 2 * dot((r.o - o), r.d);
    double c = (r.o - o).norm2() - r2;

    double test = b * b - 4 * a * c;
    double t1 = r.min_t - 1;
    double t2 = r.max_t + 1;

    if (test >= 0) {
        t2 = (-b + sqrt(test)) / (2 * a);
        t1 = (-b - sqrt(test)) / (2 * a);
    }

    return ((t1 >= r.min_t) && (t1 <= r.max_t)) ||
           ((t2 >= r.min_t) && (t2 <= r.max_t));
}

bool Sphere::intersect(const Ray &r, Intersection *i) const {
    // TODO (Part 1.4):
    // Implement ray - sphere intersection.
    // Note again that you might want to use the the Sphere::test helper here.
    // When an intersection takes place, the Intersection data should be updated
    // correspondingly.
    double a = (r.d).norm2();
    double b = 2 * dot((r.o - o), r.d);
    double c = (r.o - o).norm2() - r2;

    double test = b * b - 4 * a * c;
    double t1 = r.min_t - 1;
    double t2 = r.max_t + 1;

    double t = 0;
    double flag = false;

    if (test >= 0) {
        t2 = (-b + sqrt(test)) / (2 * a);
        t1 = (-b - sqrt(test)) / (2 * a);

        if (r.min_t <= t1 && t1 <= r.max_t) {
            t = t1;
            flag = true;
            r.max_t = t;
            i->t = t;
            i->n = (r.o + t * r.d - o);
            i->n.normalize();
            i->primitive = this;
            i->bsdf = get_bsdf();
        } else if (r.min_t <= t2 && t2 <= r.max_t) {
            t = t2;
            flag = true;
            r.max_t = t;
            i->t = t;
            i->n = (r.o + t * r.d - o);
            i->n.normalize();
            i->primitive = this;
            i->bsdf = get_bsdf();
        }
    }

    return flag;
}

void Sphere::draw(const Color &c, float alpha) const {
    Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color &c, float alpha) const {
    // Misc::draw_sphere_opengl(o, r, c);
}

}  // namespace SceneObjects
}  // namespace CGL
