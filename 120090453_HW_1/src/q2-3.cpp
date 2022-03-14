#include<iostream>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/SVD>
#include<eigen3/Eigen/Dense>
#include<opencv2/opencv.hpp>
#include<opencv2/core/eigen.hpp>
using namespace std;
using namespace cv;
using namespace Eigen;
using namespace Eigen::internal;  
using namespace Eigen::Architecture;  

int q2_3(char* argv[]){
    Mat originalImg = imread("lenna.png");
    Mat grayImg;
    Mat_<float> normalizedImg;
    //originalImg *= 1./255;
    cvtColor(originalImg, grayImg, COLOR_RGB2GRAY);
    imwrite("./grayImg.png", grayImg);
    normalize(grayImg, normalizedImg, 0, 1, NORM_MINMAX);
    imwrite("./nomalizedImg.png", normalizedImg);
    MatrixXf img, U, S, V;
    cv2eigen(normalizedImg, img);
    JacobiSVD<MatrixXf> svd(img, ComputeThinU | ComputeThinV);
    V = svd.matrixV(), U = svd.matrixU();
    S = U.inverse() * img * V.transpose().inverse();
    MatrixXf USV1, USV10, USV50;
    USV1 = U * S.block(0, 0, 512, 1) * V.block(0, 0, 512, 1).transpose();
    USV10 = U * S.block(0, 0, 512, 10) * V.block(0, 0, 512, 10).transpose();
    USV50 = U * S.block(0, 0, 512, 50) * V.block(0, 0, 512, 50).transpose();
    Mat_<float> cvUSV1, cvUSV10, cvUSV50;
    eigen2cv(USV1, cvUSV1);
    eigen2cv(USV10, cvUSV10);
    eigen2cv(USV50, cvUSV50);
    cvUSV1 *= 255;
    cvUSV10 *= 255;
    cvUSV50 *= 255;
    imwrite("./USV1.png", cvUSV1);
    imwrite("./USV10.png", cvUSV10);
    imwrite("./USV50.png", cvUSV50);
    cout << cvUSV50;
    return 3;
}