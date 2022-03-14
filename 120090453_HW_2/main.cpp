#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <eigen3/Eigen/Geometry> 
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>
#include <cmath>
// add some other header files you need

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0],
                0, 1, 0, -eye_pos[1],
                0, 0, 1, -eye_pos[2],
                0, 0, 0, 1;

    view = translate * view;
    //std::clog << "view" << std::endl << view << std::endl;  // check data

    return view;
}


Eigen::Matrix4f get_model_matrix(float rotation_angle,
                                Eigen::Vector3f T,
                                Eigen::Vector3f S,
                                Eigen::Vector3f P0,
                                Eigen::Vector3f P1)
{
    Eigen::Matrix4f model=Eigen::Matrix4f::Identity();
    //Step 1: Build the Translation Matrix M_trans:
    Eigen::Matrix4f M_trans;
    M_trans << 1, 0, 0, T[0],
                0, 1, 0, T[1],
                0, 0, 1, T[2],
                0, 0, 0, 1;
    //Step 2: Build the Scale Matrix S_trans:
    Eigen::Matrix4f S_trans;
    S_trans << S[0], 0, 0, 0,
                0, S[1], 0, 0,
                0, 0, S[2], 0,
                0, 0, 0, 1;

    //Step 3: Implement Rodrigues' Rotation Formular, rotation by angle theta around axix u, then get the model matrix
    Eigen::Vector3f axis = P1-P0;
    
    Eigen::Matrix4f rotation = Eigen::Matrix4f::Identity();
    float radians = (MY_PI * rotation_angle) / 180;

    Eigen::Matrix3f N, R;
    N << 0, -axis[2], -axis[1],
        axis[2], 0, -axis[0],
        axis[1], axis[0], 0;
    
    Eigen::Matrix3f I = Eigen::Matrix3f::Identity();

    R = (1 - cosf(radians)) * N * N + sinf(radians) * N + I;

    rotation << R(0, 0), R(0, 1), R(0, 2), 0,
                R(1, 0), R(1, 1), R(1, 2), 0,
                R(2, 0), R(2, 1), R(2, 2), 0,
                0, 0, 0, 1;

	// The axis u is determined by two points, u = P1-P0: Eigen::Vector3f P0 ,Eigen::Vector3f P1  
    // Create the model matrix for rotating the triangle around a given axis. // Hint: normalize axis first
    
    Eigen::AngleAxisf rotation1(radians, axis);
    //std::cout << std::endl << (R==rotation1.toRotationMatrix()) << std::endl; 

	//Step 4: Use Eigen's "AngleAxisf" to verify your Rotation
	//Eigen::AngleAxisf rotation_vector(radian, Vector3f(axis[0], axis[1], axis[2]));  
	//Eigen::Matrix3f rotation_matrix;
	//rotation_m = rotation_vector.toRotationMatrix();
    model = rotation * S_trans * M_trans * model;
	return model;
}



Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    Eigen::Matrix4f projection;

    // Implement this function

    projection << 1.0/(aspect_ratio*tanf(eye_fov/2)), 0, 0, 0,
                    0, 1.0/(tanf(eye_fov/2)), 0, 0,
                    0, 0, (-zFar)/(zNear-zFar), (zNear*zFar)/(zNear-zFar),
                    0, 0, 1, 0;

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    // frustum -> cubic

    // orthographic projection

    // squash all transformations

    // std::clog << "projection" << std::endl << projection << std::endl; //check

    return projection;
}

int main(int argc, const char** argv)
{   
    /*
    std::cout << "Argc: " << argc << std::endl;
    for(int i=0; i<argc; i++){
        std::cout << "Arg_i: " << argv[i] << std::endl;
    }
    std::cout << (std::string(argv[1])=="-GUI")<< std::endl;
    */
    float angle = 0;
    Eigen::Vector3f axis(0, 0, 1);
    bool command_line = false;
    std::string filename = "result.png";
    if (argc >= 2){
        if (std::string(argv[1])=="-GUI" || std::string(argv[1])=="-r"){  // -r by default
            if (argc == 2) {

            }
            else if (argc == 3) {
                angle = std::atof(argv[2]);
            }
            else if (argc == 6) {
                angle = std::atof(argv[2]);
                axis << std::atof(argv[3]), std::atof(argv[4]), std::atof(argv[5]);
            }
            else
                return 0;
        }
        else if (std::string(argv[1])=="-s"){
            command_line = true;
            if (argc == 2) {

            }
            else if (argc == 3)
                filename = std::string(argv[2]);
            else if (argc == 4) {
                filename = std::string(argv[2]);
                angle = std::stof(argv[3]);
            }
            else if (argc == 7) {
                filename = std::string(argv[2]);
                angle = std::stof(argv[3]);
                axis << std::atof(argv[4]), std::atof(argv[5]), std::atof(argv[6]);
            }
            else
                return 0;
        }
        else
            return 0;
    }

    rst::rasterizer r(1024, 1024);
    // define your eye position "eye_pos" to a proper position
    Eigen::Vector3f eye_pos(0, 0, -1);

    // define a triangle named by "pos" and "ind"
    std::vector<Eigen::Vector3f> pos;
    std::vector<Eigen::Vector3i> ind;
    pos.insert(pos.end(), Eigen::Vector3f(0, 0, 0));
    pos.insert(pos.end(), Eigen::Vector3f(1, 0, 0));
    pos.insert(pos.end(), Eigen::Vector3f(0, 1, 0));
    ind.insert(ind.end(), Eigen::Vector3i(0, 1, 2));
    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    // added parameters for get_projection_matrix(float eye_fov, float aspect_ratio,float zNear, float zFar)
    float eye_fov=MY_PI/2, aspect_ratio=1, zNear=0, zFar=5;
    Eigen::Vector3f T(0, 0, 0), S(1, 1, 1), P0(0, 0, 0), P1=axis.normalized();

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        
        r.set_model(get_model_matrix(angle, T, S, P0, P1));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(eye_fov, aspect_ratio, zNear, zFar));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        //cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        cv::Mat image(1024, 1024, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);
        /*
        
        std::cout << "Angle: "<< angle << std::endl;
        std::cout << "Axis: \n"<< axis << std::endl;
        */
        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle, T, S, P0, P1));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(eye_fov, aspect_ratio, zNear, zFar));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(1024, 1024, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';
        std::cout << "angle: " << angle << std::endl;
    

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }
    return 0;
}

/*
result 1:
    eye_fov=Pi/3,
    eye_fov=MY_PI/2, aspect_ratio=1, zNear=0, zFar=5,
    angle=0

result 2:
    eye_fov=Pi/3,
    eye_fov=MY_PI/2, aspect_ratio=1, zNear=0, zFar=5,
    axis=(1, 0, 0),
    angle=60

result 3:
    eye_fov=Pi/3,
    eye_fov=MY_PI/2, aspect_ratio=1, zNear=0, zFar=5,
    axis=(1, 1, 1),
    angle=60
*/