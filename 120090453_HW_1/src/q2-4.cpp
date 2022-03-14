#include<iostream>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Eigen>
#include<eigen3/Eigen/Geometry>
using namespace std;
using namespace Eigen;

int q2_4(char* argv[]){
    Vector3f v(1, 2, 3), w(4, 5, 6);
    AngleAxisf rotation1(M_PI/4, Vector3f(1, 0, 0));
    AngleAxisf rotation2(M_PI/6, Vector3f(0, 1, 0));
    AngleAxisf rotation3(M_PI/3, Vector3f(0, 0, 1));
    Matrix3f rotationM1 = rotation1.matrix();
    MatrixXf rotationM2 = rotation2.matrix();
    MatrixXf rotationM3 = rotation3.matrix();
    Vector3f v1 = rotationM3 * rotationM2 * rotationM1 * (v-w) + w;
    cout << "New Point:\n" << v1.transpose() << endl;
    return 4;
}