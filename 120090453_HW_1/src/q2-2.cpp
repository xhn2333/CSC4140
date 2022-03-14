#include<iostream>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/SVD>
#include<cmath>
using namespace std;
using namespace Eigen;



int q2_2(char* argv[]){
    Matrix4f i, j;
    i<< 1, 2, 3, 4,
        5, 6, 7, 8,
        9,10,11,12,
       13,14,15,16;
    j<< 4, 3, 2, 1,
        8, 7, 5, 6,
       12,11,10, 9,
       16,15,14,13;
    cout << "matrix i:\n" << i << endl;
    cout << "matrix j:\n" << j << endl;
    cout << "matrix add i+j:\n" << i+j <<endl;
    cout << "matrix multiply i*j:\n" << i*j << endl;
    Vector4f v(1, 1.5, 2, 3);
    cout << "matrix multiply vector i*v:\n" << i*v << endl;
    return 2;
}