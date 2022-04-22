#include "bbox.h"

#include <algorithm>
#include <iostream>

#include "GL/glew.h"

namespace CGL {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {
    // TODO (Part 2.2):
    // Implement ray - bounding box intersection test
    // If the ray intersected the bouding box within the range given by
    // t0, t1, update t0 and t1 with the new intersection times.
    Vector3D inPoint = (min - r.o) * r.inv_d;
    Vector3D outPoint = (max - r.o) * r.inv_d;
    Vector3D tmax = (-INF_D, -INF_D, -INF_D);
    Vector3D tmin = (INF_D, INF_D, INF_D);
    for (int i = 0; i < 3; i++) {
        if (inPoint[i] > outPoint[i]) {
            std::swap(inPoint[i], outPoint[i]);
        }
    }

    auto t11 =
        std::min(t1, std::min(outPoint.x, std::min(outPoint.y, outPoint.z)));
    auto t01 =
        std::max(t0, std::max(inPoint.x, std::max(inPoint.y, inPoint.z)));
    return t01 < t11;
}

void BBox::draw(Color c, float alpha) const {
    glColor4f(c.r, c.g, c.b, alpha);

    // top
    glBegin(GL_LINE_STRIP);
    glVertex3d(max.x, max.y, max.z);
    glVertex3d(max.x, max.y, min.z);
    glVertex3d(min.x, max.y, min.z);
    glVertex3d(min.x, max.y, max.z);
    glVertex3d(max.x, max.y, max.z);
    glEnd();

    // bottom
    glBegin(GL_LINE_STRIP);
    glVertex3d(min.x, min.y, min.z);
    glVertex3d(min.x, min.y, max.z);
    glVertex3d(max.x, min.y, max.z);
    glVertex3d(max.x, min.y, min.z);
    glVertex3d(min.x, min.y, min.z);
    glEnd();

    // side
    glBegin(GL_LINES);
    glVertex3d(max.x, max.y, max.z);
    glVertex3d(max.x, min.y, max.z);
    glVertex3d(max.x, max.y, min.z);
    glVertex3d(max.x, min.y, min.z);
    glVertex3d(min.x, max.y, min.z);
    glVertex3d(min.x, min.y, min.z);
    glVertex3d(min.x, max.y, max.z);
    glVertex3d(min.x, min.y, max.z);
    glEnd();
}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
    return os << "BBOX(" << b.min << ", " << b.max << ")";
}

}  // namespace CGL
