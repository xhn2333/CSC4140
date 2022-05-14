#include <fstream>
#include <iostream>
#include <sstream>

#include "CGL/misc.h"
#include "CGL/vector2D.h"
#include "CGL/vector3D.h"
#include "camera.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::max;
using std::min;
using std::ofstream;

namespace CGL {

using Collada::CameraInfo;

Ray Camera::generate_ray_for_thin_lens(double x, double y, double rndR,
                                       double rndTheta) const {
    // TODO Assignment 7: Part 4
    // compute position and direction of ray from the input sensor sample
    // coordinate. Note: use rndR and rndTheta to uniformly sample a unit disk.
    x -= 0.5;
    y -= 0.5;

    x *= 2 * tan(radians(hFov / 2));
    y *= 2 * tan(radians(vFov / 2));
    // cout << x << " " << y << endl;
    Ray r;
    r.min_t = nClip;
    r.max_t = fClip;

    Vector3D pLens(lensRadius * sqrt(rndR) * cos(rndTheta),
                   lensRadius * sqrt(rndR) * sin(rndTheta), 0);
    Vector3D pFocus(x * focalDistance, y * focalDistance, -focalDistance);
    r.d = c2w * (pFocus - pLens);
    r.d.normalize();
    r.o = pos + c2w * pLens;
    return r;
}

}  // namespace CGL
