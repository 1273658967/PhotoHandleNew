//
// Created by eg on 2020/6/12.
//
#include "equalscaleimg.h"

void getmm2pixel(vector<Point2f> image_points_buf, double& mm2pixel_w,
                 double& mm2pixel_h, float& chess_width, float& chess_height)
{
    vector<Point2f> pnts(4);

    //get points in the world
    float dis_w = 0;
    //for(int i = 0; i < 4; i++)
    {
        pnts[0] = image_points_buf[5];
        pnts[1] = image_points_buf[8];

        dis_w =sqrtf((pnts[0].x - pnts[1].x)*(pnts[0].x - pnts[1].x) + (pnts[0].y - pnts[1].y)*(pnts[0].y - pnts[1].y));
    }

    //dis_w = dis_w/11;
    //计算参考平面内每像素对应的真实物理尺寸（mm/pixel）
    mm2pixel_w = 46/dis_w;
    //mm2pixel_w = 30/dis_w;

    pnts[2] = image_points_buf[5];
    pnts[3] = image_points_buf[41];
    mm2pixel_h = 45/sqrtf((pnts[2].x - pnts[3].x)*(pnts[2].x - pnts[3].x) + (pnts[2].y - pnts[3].y)*(pnts[2].y - pnts[3].y));
    //mm2pixel_h = 30/sqrtf((pnts[2].x - pnts[3].x)*(pnts[2].x - pnts[3].x) + (pnts[2].y - pnts[3].y)*(pnts[2].y - pnts[3].y));
    cout<<"mm2pixel_w = "<<mm2pixel_w<<" mm2pixel_h = "<<mm2pixel_h<<endl;

    //尝试计算实际距离
    chess_height = 0;
    chess_width = 0;

    //chess_width += mm2pixel_w*sqrtf((image_points_buf[START_TOP].x - image_points_buf[END_TOP].x)*(image_points_buf[START_TOP].x - image_points_buf[END_TOP].x)
    //+ (image_points_buf[START_TOP].y - image_points_buf[END_TOP].y)*(image_points_buf[START_TOP].y - image_points_buf[END_TOP].y));

    chess_width += mm2pixel_w*sqrtf((image_points_buf[START_BOTTOM].x - image_points_buf[END_BOTTOM].x)*(image_points_buf[START_BOTTOM].x - image_points_buf[END_BOTTOM].x)
                                    + (image_points_buf[START_BOTTOM].y - image_points_buf[END_BOTTOM].y)*(image_points_buf[START_BOTTOM].y - image_points_buf[END_BOTTOM].y));
    // chess_width = chess_width/2;

    chess_height += mm2pixel_h*sqrtf((image_points_buf[START_TOP].x - image_points_buf[START_BOTTOM].x)*(image_points_buf[START_TOP].x - image_points_buf[START_BOTTOM].x)
                                     + (image_points_buf[START_TOP].y - image_points_buf[START_BOTTOM].y)*(image_points_buf[START_TOP].y - image_points_buf[START_BOTTOM].y));

    chess_height += mm2pixel_h*sqrtf((image_points_buf[END_TOP].x - image_points_buf[END_BOTTOM].x)*(image_points_buf[END_TOP].x - image_points_buf[END_BOTTOM].x)
                                     + (image_points_buf[END_TOP].y - image_points_buf[END_BOTTOM].y)*(image_points_buf[END_TOP].y - image_points_buf[END_BOTTOM].y));
    chess_height = chess_height/2;
    cout<<"chess width = "<<chess_width<<" chess height = "<<chess_height<<endl;
}

void getvincodeimg(Mat img_rotate, Rect roi, double mm2pixel_w, double mm2pixel_h, Mat& A4_Img, float chess_width, float chess_height)
{
    roi &= Rect(0, 0, img_rotate.cols, img_rotate.rows);
    Mat Printimg = img_rotate(roi);
    //imshow("Printimg", Printimg);

    cout<<"printimg.cols = "<<Printimg.cols<<"  printimg.rows = "<<Printimg.rows<<endl;

    //计算实际ROI区域图像计算实际物理尺寸mm
    double world_width = Printimg.cols*mm2pixel_w;
    double world_height = Printimg.rows*mm2pixel_h;
    cout<<"world_width(mm) = "<<world_width<<"  world_height(mm) = "<<world_height<<endl;

    //利用棋盘格标准尺寸长度进行校准
    double diff_w = chess_width - STAND_WIDTH;
    double diff_h = chess_height - STAND_HEIGHT;
    if(diff_w > 0)//测量值比实际值大
    {
        world_width = world_width + diff_w;
    }
    else //测量值比实际值小
    {
        world_width = world_width - diff_w;
    }

    if(diff_h > 0)//测量值比实际值大
    {
        world_height = world_height + diff_h;
    }
    else //测量值比实际值小
    {
        world_height = world_height - diff_h;
    }
    cout<<"after world_width = "<<world_width<<"  after world_height = "<<world_height<<endl;


    //图像打印成297mmX210mm的A4纸时,每个像素对应的物理尺寸为
    double p_mm2pixel_width =  297.0f/A4WIDTH;
    double p_mm2pixel_height = 210.0f/A4HEIGHT;

    //要保持被打印物体图像在A4纸上打印后物理尺寸不变,则需要对应像素转换
    //计算实际物理图像映射到A4纸上时,在842X595图像上的像素数
    int pixels_A4_w = cvCeil(world_width/p_mm2pixel_width);
    int pixels_A4_h = cvCeil(world_height/p_mm2pixel_height);
    cout<<"pixels_A4_w(pixels) = "<<pixels_A4_w<<" pixels_A4_h(pixels) = "<<pixels_A4_h<<endl;

    //然后对要打印图像进行缩放,然后copy到842X595图像上
    Mat tmp;
    resize(Printimg, tmp, Size(pixels_A4_w, pixels_A4_h), 3);
    //cv::imwrite("Printimg.jpg", tmp);

    //利用第三方保存VINcode
    //SetResolution(tmp);

    //copy到842X595图像上
    if(tmp.cols > A4_Img.cols && tmp.rows > A4_Img.rows)
    {
        for(int i = 0; i < A4_Img.rows; i++)
        {
            for(int j = 0; j < A4_Img.cols; j++)
            {
                *(A4_Img.data + i*A4_Img.cols*3 + j*3 + 0) = *(tmp.data + i*tmp.cols*3 + j*3 + 0);
                *(A4_Img.data + i*A4_Img.cols*3 + j*3 + 1) = *(tmp.data + i*tmp.cols*3 + j*3 + 1);
                *(A4_Img.data + i*A4_Img.cols*3 + j*3 + 2) = *(tmp.data + i*tmp.cols*3 + j*3 + 2);
            }
        }
    }
    else if(tmp.cols > A4_Img.cols && tmp.rows < A4_Img.rows)
    {
        for(int i = 0; i < tmp.rows; i++)
        {
            for(int j = 0; j < A4_Img.cols; j++)
            {
                *(A4_Img.data + i*A4_Img.cols*3 + j*3 + 0) = *(tmp.data + i*tmp.cols*3 + j*3 + 0);
                *(A4_Img.data + i*A4_Img.cols*3 + j*3 + 1) = *(tmp.data + i*tmp.cols*3 + j*3 + 1);
                *(A4_Img.data + i*A4_Img.cols*3 + j*3 + 2) = *(tmp.data + i*tmp.cols*3 + j*3 + 2);
            }
        }
    }
    else if(tmp.cols < A4_Img.cols && tmp.rows > A4_Img.rows)
    {
        for(int i = 0; i < A4_Img.rows; i++)
        {
            for(int j = 0; j < tmp.cols; j++)
            {
                *(A4_Img.data + i*A4_Img.cols*3 + j*3 + 0) = *(tmp.data + i*tmp.cols*3 + j*3 + 0);
                *(A4_Img.data + i*A4_Img.cols*3 + j*3 + 1) = *(tmp.data + i*tmp.cols*3 + j*3 + 1);
                *(A4_Img.data + i*A4_Img.cols*3 + j*3 + 2) = *(tmp.data + i*tmp.cols*3 + j*3 + 2);
            }
        }
    }
    else
    {
        for(int i = 0; i < tmp.rows; i++)
        {
            for(int j = 0; j < tmp.cols; j++)
            {
                *(A4_Img.data + i*A4_Img.cols*3 + j*3 + 0) = *(tmp.data + i*tmp.cols*3 + j*3 + 0);
                *(A4_Img.data + i*A4_Img.cols*3 + j*3 + 1) = *(tmp.data + i*tmp.cols*3 + j*3 + 1);
                *(A4_Img.data + i*A4_Img.cols*3 + j*3 + 2) = *(tmp.data + i*tmp.cols*3 + j*3 + 2);
            }
        }
    }
}


