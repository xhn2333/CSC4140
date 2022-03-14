#include<iostream>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/SVD>
#include<eigen3/Eigen/Geometry>
using namespace std;
using namespace Eigen;

int q2_1(char* argv[]){
    Vector4f v(1, 1.5, 2, 3), w(0, 1, 2, 4), ans(0, 0, 0, 1);
    Vector3f v_3, w_3;
    v_3=v.hnormalized(), w_3=w.hnormalized();
    cout << "\nvector v=\n" << v_3.homogeneous() << endl;
    cout << "vector w=\n" << w_3.homogeneous() << endl;

    cout << "\nv + w=\n" << (v_3+w_3).homogeneous() << endl;
    cout << "\nv dot w=\n" << v_3.dot(w_3) << endl;
    cout << "\nv cross w=\n" << (v_3.cross(w_3)).homogeneous() << endl;

    return 1;
}