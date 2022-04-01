#ifndef STUDENT_CODE_H
#define STUDENT_CODE_H

#include "halfEdgeMesh.h"
#include "bezierPatch.h"
#include "bezierCurve.h"

using namespace std;

namespace CGL
{

    class MeshResampler
    {

    public:
        MeshResampler(){};
        ~MeshResampler() {}
        void upsample(HalfedgeMesh &mesh);
    };

    class Utils{

    public:
        static double BernsteinPolynomial1D(int n, long j, double t);
        static double BernsteinPolynomial(int m, int n, long i, long j, double u, double v);
    };
}

#endif // STUDENT_CODE_H
