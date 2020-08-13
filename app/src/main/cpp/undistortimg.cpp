#include "undistortimg.h"
#define FORCUS 1283.04
//#include "opencv2/stitching.hpp"

///* 摄像机内参数矩阵 2048X1536  oldPDA-1/10mm  xxx (MATLAB标定参数)15CM   */
//Mat cameraMatrix = (Mat_<double>( 3, 3) << 265.793138104148, 0, 1045.50911518944, 0, 265.890845005617, 798.926271646318, 0, 0, 1);
///* 摄像机的5个畸变系数：k1,k2,p1,p2,k3 */
//Mat distCoeffs = (Mat_<double>( 1, 4) << 0.00386766944022231, -0.000292143385810783, 0, 0);

/* 摄像机内参数矩阵 1600*1200  oldPDA-/10mm  2001 (MATLAB标定参数)15CM   */
Mat cameraMatrix = (Mat_<double>( 3, 3) << 1770.40946708180, 0, 812.202584080922, 0, 1770.40580082803, 626.108962293626, 0, 0, 1);
/* 摄像机的5个畸变系数：k1,k2,p1,p2,k3 */
Mat distCoeffs = (Mat_<double>( 1, 5) << 0.238894724495483, -0.956230921540142, 0, 0  ,-1.63427917642147);

// Checks if a matrix is a valid rotation matrix.
bool isRotationMatrix(Mat &R)
{
    Mat Rt;
    transpose(R, Rt);
    Mat shouldBeIdentity = Rt * R;
    Mat I = Mat::eye(3,3, shouldBeIdentity.type());
    return  norm(I, shouldBeIdentity) < 1e-6;

}

// Calculates rotation matrix to euler angles
// The result is the same as MATLAB except the order
// of the euler angles ( x and z are swapped ).
Vec3f rotationMatrixToEulerAngles(Mat &R)
{

    assert(isRotationMatrix(R));

    float sy = sqrt(R.at<double>(0,0) * R.at<double>(0,0) +  R.at<double>(1,0) * R.at<double>(1,0) );

    bool singular = sy < 1e-6; // If

    float x, y, z;
    if (!singular)
    {
        x = atan2(R.at<double>(2,1) , R.at<double>(2,2));
        y = atan2(-R.at<double>(2,0), sy);
        z = atan2(R.at<double>(1,0), R.at<double>(0,0));
    }
    else
    {
        x = atan2(-R.at<double>(1,2), R.at<double>(1,1));
        y = atan2(-R.at<double>(2,0), sy);
        z = 0;
    }
    return Vec3f(x, y, z);
}

bool getEulerangle(Mat img, vector<Point3d> point3D, vector<Point2d> image_points, Vec3f& euler, float&dist)
{
    bool success = false;

    cv::Mat rotation_vector = cv::Mat::zeros( 3, 1, CV_64FC1);
    cv::Mat translation_vector = cv::Mat::zeros( 3, 1, CV_64FC1);

    // Solve for pose
    cv::solvePnP(point3D, image_points, cameraMatrix, distCoeffs, rotation_vector, translation_vector);

    dist = 110*FORCUS/abs(image_points[0].x - image_points[1].x);

    //旋转向量-->旋转矩阵
    Mat rotate_Matrix = Mat(3, 3, CV_64FC1);  // 图像的旋转矩阵
    Rodrigues(rotation_vector, rotate_Matrix);

    //计算相机旋转角
    double theta_x, theta_y,theta_z;
    double PI = 3.1415926;
    theta_x = atan2(rotate_Matrix.at<double>(2, 1), rotate_Matrix.at<double>(2, 2));
    theta_y = atan2(-rotate_Matrix.at<double>(2, 0),
                    sqrt(rotate_Matrix.at<double>(2, 1)*rotate_Matrix.at<double>(2, 1) + rotate_Matrix.at<double>(2, 2)*rotate_Matrix.at<double>(2, 2)));
    theta_z = atan2(rotate_Matrix.at<double>(1, 0), rotate_Matrix.at<double>(0, 0));
    theta_x = theta_x * (180 / PI);
    theta_y = theta_y * (180 / PI);
    theta_z = theta_z * (180 / PI);

    //计算深度
    Mat P;
    P = -(rotate_Matrix.t()) * translation_vector;
    cout<<"tran_vector:"<<translation_vector<<endl;

    //输出
    cout<<"角度"<<endl;
    cout<<theta_x<<endl;
    cout<<theta_y<<endl;
    cout<<theta_z<<endl;
    cout<<P<<endl;

    //旋转矩阵-->欧拉角
    //euler = rotationMatrixToEulerAngles(rotate_Matrix);
    euler[0] = theta_x;
    euler[1] = theta_y;
    euler[2] = theta_z;

    // Project a 3D point onto the image plane.
    vector<cv::Point2d> point2D;
    projectPoints(point3D, rotation_vector, translation_vector, cameraMatrix, distCoeffs, point2D);

    cv::line(img, image_points[0], point2D[0], cv::Scalar(255,0,0), 3);
    cv::line(img, point2D[0], point2D[1], cv::Scalar(0,0,255), 3);
    cv::line(img, point2D[0], point2D[2], cv::Scalar(0,255,0), 3);

    return success;
}

bool  Imgundistort(Mat distortimg, Mat& undistortimg, int dstwidth, int dstheight)
{
    bool flag = false;

#if 0

    float scale_w = 0.9f;

    //step1:先将图像左右有交叠的分成两份
    Mat BlockImg_0 = distortimg(Rect(0, 0, distortimg.cols*scale_w, distortimg.rows));
    Mat BlockImg_1 = distortimg(Rect(distortimg.cols*(1 - scale_w), 0, distortimg.cols*scale_w, distortimg.rows));

#if 0

    int width = BlockImg_0.cols*scale_w;
    int height = BlockImg_0.rows;

    Mat mapx = Mat(Size(width, height),CV_32FC1);
    Mat mapy = Mat(Size(width, height),CV_32FC1);
    Mat R = Mat::eye(3,3,CV_32F);

    //step2:将左右两部分图像再各自有交叠的分成两部分
    Mat BlockImg_0_0 = BlockImg_0(Rect(0, 0, BlockImg_0.cols*scale_w, BlockImg_0.rows));
    Mat BlockImg_0_1 = BlockImg_0(Rect(BlockImg_0.cols*(1-scale_w), 0, BlockImg_0.cols*scale_w, BlockImg_0.rows));
    initUndistortRectifyMap(cameraMatrix,distCoeffs,R,getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, Size(width, height),
    1, Size(width, height), 0),Size(width, height),CV_32FC1,mapx,mapy);

    //step3:分别矫正
    Mat BlockImg_0_0_undistortimg;
    Mat BlockImg_0_1_undistortimg;
    remap(BlockImg_0_0,BlockImg_0_0_undistortimg,mapx, mapy, INTER_AREA);
    remap(BlockImg_0_1,BlockImg_0_1_undistortimg,mapx, mapy, INTER_AREA);

    //矫正右半部分两块
    //initUndistortRectifyMap(cameraMatrix,distCoeffs,R,cameraMatrix,Size(width, height),CV_32FC1,mapx,mapy);

    Mat BlockImg_1_0 = BlockImg_1(Rect(0, 0, BlockImg_1.cols*scale_w, BlockImg_1.rows));
    Mat BlockImg_1_1 = BlockImg_1(Rect(BlockImg_1.cols*(1-scale_w), 0, BlockImg_1.cols*scale_w, BlockImg_1.rows));

    Mat BlockImg_1_0_undistortimg;
    Mat BlockImg_1_1_undistortimg;
    remap(BlockImg_1_0,BlockImg_1_0_undistortimg,mapx, mapy, INTER_AREA);
    remap(BlockImg_1_1,BlockImg_1_1_undistortimg,mapx, mapy, INTER_AREA);

    //分别拼接左右两部分
    Mat BlockImg_0_stitch(Size(width, height), CV_8UC3);
    bool flag_0 = stitch_img(BlockImg_0_1_undistortimg, BlockImg_0_0_undistortimg, BlockImg_0_stitch, width, height);
    if(flag_0 == false)
    {
        cout<<"stitch0_0_1 is failed"<<endl;
    }

    Mat BlockImg_1_stitch(Size(width, height), CV_8UC3);
    bool flag_1 = stitch_img(BlockImg_1_1_undistortimg, BlockImg_1_0_undistortimg, BlockImg_1_stitch, width, height);
    if(flag_1 == false)
    {
        cout<<"stitch1_0_1 is failed"<<endl;
    }

    //最终拼接左右两部分
    //拼接左右两部份分块矫正过的图像
    flag = stitch_img(BlockImg_1_stitch, BlockImg_0_stitch, undistortimg, dstwidth, dstheight);
    if(flag == false)
    {
        cout<<"stitch is failed"<<endl;
    }


    imshow("0_0",BlockImg_0_0_undistortimg);
    imshow("0_1",BlockImg_0_1_undistortimg);
    imshow("1_0",BlockImg_1_0_undistortimg);
    imshow("1_1",BlockImg_1_1_undistortimg);
    imshow("stitch_0", BlockImg_0_stitch);
    imshow("stitch_1", BlockImg_1_stitch);
    #else
    int width = distortimg.cols*scale_w;
    int height = distortimg.rows;

    Mat mapx = Mat(Size(width, height),CV_32FC1);
    Mat mapy = Mat(Size(width, height),CV_32FC1);
    Mat R = Mat::eye(3,3,CV_32F);

    //initUndistortRectifyMap(cameraMatrix,distCoeffs,R,cameraMatrix,Size(width, height),CV_32FC1,mapx,mapy);
    initUndistortRectifyMap(cameraMatrix,distCoeffs,R,getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, Size(width, height), 1, Size(width, height), 0)
    ,Size(width, height),CV_32FC1,mapx,mapy);

    Mat BlockImg_0_undistortimg;
    Mat BlockImg_1_undistortimg;
    remap(BlockImg_0,BlockImg_0_undistortimg,mapx, mapy, INTER_AREA);
    remap(BlockImg_1,BlockImg_1_undistortimg,mapx, mapy, INTER_AREA);

    imshow("src0",BlockImg_0);
    imshow("src1",BlockImg_1);
    imshow("0",BlockImg_0_undistortimg);
    imshow("1",BlockImg_1_undistortimg);

    //拼接左右两部份分块矫正过的图像
    flag = stitch_img(BlockImg_1_undistortimg, BlockImg_0_undistortimg, undistortimg, dstwidth, dstheight);
    if(flag == false)
    {
        cout<<"stitch is failed"<<endl;
    }

    #endif

#else
    undistort(distortimg,undistortimg,cameraMatrix,distCoeffs);

    flag = true;

#endif

    return flag;
}