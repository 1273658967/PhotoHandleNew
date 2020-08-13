#include <android/log.h>
//#define TAG "vin_tag"
//#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#include "log.h"
#include "undistortimg.h"
#include "findcorners.h"
#include "equalscaleimg.h"
#include <opencv2/imgcodecs.hpp>

int VinMeasureProcess(Mat rgbImg, Mat* vinImg)
{
    int code;
    int total = 0;
    float rotate_angle = 0;
    double mm2pixel_w, mm2pixel_h;
    float chess_width, chess_height;
    float diff_width = 0;
    float diff_height = 0;

    vector<Point2f> image_points_buf;  /* 缓存每幅图像上检测到的角点 */
    Rect roi;

    if(rgbImg.empty())
    {
        LOGE("input src image is empty\n");
        return -1;
    }

    LOGE("我来了\n");
    imwrite("/storage/emulated/0/VIN/input.png", rgbImg);

    if(rgbImg.cols == 0 || rgbImg.rows == 0 || rgbImg.data == NULL)
    {
        LOGE("Input image is failed!\n");
        return -1;
    }

    Mat image = rgbImg.clone();
    Rect SearchRegion;

    //第一次提取角点
    bool first_success  = extractcorners(image, image_points_buf);
    if(true == first_success)
    {
        LOGE("第一次提取角点成功\n");
        //求取旋转角度
        rotate_angle = calcrotateangle(image_points_buf);

        total = image_points_buf.size();

        //旋转图像
        Mat img_rotate = rotateimg(image, rotate_angle);

        //在旋转后的图像上进行ROI区域设定  //==!!!!!注意相机与标定板的距离==15cm
        // 300万像素
//        SearchRegion.x = max(int(image_points_buf[START_TOP].x - 3*(image_points_buf[END_TOP].x - image_points_buf[START_TOP].x)/12), 0);
//        SearchRegion.y = 0;
//        SearchRegion.width = abs(image_points_buf[total-1].x - 3*image_points_buf[START_TOP].x)+15*abs(image_points_buf[END_TOP].x - image_points_buf[START_TOP].x)/12;
//        SearchRegion.height = img_rotate.rows*0.9;

        //200万像素
        SearchRegion.x = max(int(image_points_buf[START_TOP].x - 3*(image_points_buf[END_TOP].x - image_points_buf[START_TOP].x)/12), 0);
        SearchRegion.y = 0;
        SearchRegion.width = abs(image_points_buf[total-1].x - 2*image_points_buf[START_TOP].x)+15*abs(image_points_buf[END_TOP].x - image_points_buf[START_TOP].x)/12;
        SearchRegion.height = img_rotate.rows*0.9;

        SearchRegion &=Rect(0, 0, img_rotate.cols, img_rotate.rows);
        Mat distortimg = img_rotate(SearchRegion);
        Mat undistortimg(distortimg.size(), distortimg.type());

        //矫正图像
        bool undistortflag = Imgundistort(distortimg, undistortimg, undistortimg.cols, undistortimg.rows);
        if(undistortflag)
        {
            LOGE("矫正成功\n");
            cv::imwrite("/storage/emulated/0/VIN/undistortimg.png", undistortimg);

            //第二次在旋转后的图像上提取角点
            bool second_success = extractcorners(undistortimg, image_points_buf);

            LOGE("second_success = %d\n",second_success);
            if(true == second_success)
            {
                LOGE("第二次提取角点成功\n");
                //计算每个像素对应的真实尺寸
                getmm2pixel(image_points_buf, mm2pixel_w, mm2pixel_h, chess_width, chess_height);
                diff_width = abs(chess_width - STAND_WIDTH);
                diff_height = abs(chess_height - STAND_HEIGHT);

                LOGE("diff_width = %f  diff_height = %f\n", diff_width, diff_height);
                if(diff_height < 1.0 && diff_width < 2.0)
                {
                    //按等比例投射到A4纸（297X210mm)上
                    //(600像素/英寸对应分辨率为4961X7016)
                    //(300像素/英寸对应分辨率为2480.31X3507.867)
                    Mat A4_Img(A4HEIGHT, A4WIDTH, CV_8UC3);

                    A4_Img.setTo(255);

                    //根据旋转后参照物扣取有效区域
                    cout<<"--------------------------1VS1生成图片------------------------"<<endl;
                    {
                        roi.x = max((int)(image_points_buf[START_TOP].x - 5*abs(image_points_buf[END_TOP].x - image_points_buf[START_TOP].x )/12), 0);
                        roi.width = abs(image_points_buf[END_TOP].x - 1.2*image_points_buf[START_TOP].x ) + 4*abs(image_points_buf[END_TOP].x - image_points_buf[START_TOP].x )/12;
                        roi.height = image_points_buf[START_BOTTOM].y - image_points_buf[START_TOP].y;
                        roi.y = image_points_buf[START_TOP].y - roi.height*1.4;
                    }

                    //生成等比例VIN码图像,在A4纸上打印
                    getvincodeimg(undistortimg, roi, mm2pixel_w, mm2pixel_h, A4_Img, chess_width, chess_height);
                    *vinImg = A4_Img.clone();
                    LOGE("生成VIN图片成功\n");
                    code = 1;
                }
                else
                {
                    *vinImg = rgbImg.clone();
                    code = -1;
                    //LOGE("code1 = %d",code);
                }
            }else{
                *vinImg = rgbImg.clone();
                code = -1;
                //LOGE("code2 = %d",code);
            }
        }
    } else
    {
        *vinImg = rgbImg.clone();
        code = -1;
        //LOGE("code3 = %d",code);
    }
    return code;
}
