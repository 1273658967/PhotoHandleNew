#include "findcorners.h"
#include <android/log.h>
#define TAG "find_tag"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

bool extractcorners(Mat src, vector<Point2f>& image_points_buf)
{
    if(!src.empty())
    {
        bool find = findChessboardCorners(src,Size(BLOCK_SIZE_X, BLOCK_SIZE_Y),image_points_buf);
        LOGE("find = %d\n", find);
        if (!find)
        {
                cout<<"can not find chessboard corners!\n";
                LOGE("can not find chessboard corners!\n");
        }else
        {
            Mat view_gray;
            //LOGE("src.channel = %d\n", src.channels());
            if(src.channels() == 3)
            {
                cvtColor(src,view_gray,CV_RGB2GRAY);
            } else
            {
                view_gray = src.clone();
            }
            cornerSubPix(view_gray,image_points_buf,Size(11,11),Size(-1,-1),TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,30,0.1));
        }
        return find;
    }
    else{
        LOGE("input image is empty!\n");
    }
}

float calcrotateangle(vector<Point2f> image_points_buf)
{
    float angle = 0;
    float sum_theta = 0;
    int total = image_points_buf.size();

    sum_theta =atan((image_points_buf[START_TOP].y - image_points_buf[END_TOP].y) / (image_points_buf[START_TOP].x - image_points_buf[END_TOP].x));
    sum_theta +=atan((image_points_buf[START_BOTTOM].y - image_points_buf[END_BOTTOM].y) / (image_points_buf[START_BOTTOM].x - image_points_buf[END_BOTTOM].x));

    angle = 180*(sum_theta/2)/CV_PI;//转化成度单位

    return angle;
}

Mat rotateimg(Mat src, float angle)
{
    //旋转中心为图像中心
    Mat img_rotate;
    Point2f center0;

    center0.x = float(src.cols / 2.0);
    center0.y = float(src.rows / 2.0);

    //计算二维旋转的仿射变换矩阵
    Mat M = getRotationMatrix2D(center0, angle, 1);
    warpAffine(src, img_rotate, M, Size(src.cols, src.rows), 1, 0, Scalar(0,0,0));//仿射变换


    return img_rotate;
}
