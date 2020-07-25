//
// Created by lxy on 2020/6/16.
//

#ifndef PHOTOHANDLE_BITMAP_UTIL_H
#define PHOTOHANDLE_BITMAP_UTIL_H

#include <jni.h>
#include <opencv2/opencv.hpp>

using namespace cv;

extern "C" {
/**
 * Bitmap 转矩阵
 * @param env JNI环境
 * @param bitmap Bitmap对象
 * @param mat 图片矩阵
 * @param needPremultiplyAlpha 是否前乘透明度
 */
void bitmap2Mat(JNIEnv *env, jobject bitmap, Mat *mat, bool needPremultiplyAlpha = false);

/**
 * 矩阵转Bitmap
 * @param env JNI环境
 * @param mat 图片矩阵
 * @param bitmap Bitmap对象
 * @param needPremultiplyAlpha 是否前乘透明度
 */
void mat2Bitmap(JNIEnv *env, Mat mat, jobject bitmap, bool needPremultiplyAlpha = false);

/**
 *
 * 创建Bitmap
 * @param env JNI环境
 * @param src 矩阵
 * @param config Bitmap配置
 * @return Bitmap对象
 */
jobject createBitmap(JNIEnv *env, Mat src, jobject config);


}

#endif //PHOTOHANDLE_BITMAP_UTIL_H
