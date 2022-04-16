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
    Vector3D test = o - r.o;
    double a = (r.d).norm2(), b = 2 * dot(test, r.d), c = test.norm2() - r2;
    if (b * b - 4 * a * c < 0)
        return false;
    else {
        t1 = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
        t2 = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
        return true;
    }
}

bool Sphere::has_intersection(const Ray &r) const {
    // TODO (Part 1.4):
    // Implement ray - sphere intersection.
    // Note that you might want to use the the Sphere::test helper here.
    Vector3D origin = r.o;
    Vector3D s = o - origin;
    double test = s.norm2() - abs(dot(r.d, s)) * abs(dot(r.d, s));
    return test < r2;
}

bool Sphere::intersect(const Ray &r, Intersection *i) const {
    // TODO (Part 1.4):
    // Implement ray - sphere intersection.
    // Note again that you might want to use the the Sphere::test helper here.
    // When an intersection takes place, the Intersection data should be updated
    // correspondingly.

    return true;
}

void Sphere::draw(const Color &c, float alpha) const {
    Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color &c, float alpha) const {
    // Misc::draw_sphere_opengl(o, r, c);
}

}  // namespace SceneObjects
}  // namespace CGL
