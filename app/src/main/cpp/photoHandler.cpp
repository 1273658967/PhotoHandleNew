//
// Created by lxy on 2020/7/24.
//
#include <jni.h>
#include <string>
#include <iostream>
#include <android/log.h>
#include <opencv2/opencv.hpp>

#include "photoHandler.h"
#include "log.h"
#include "bitmap-util.h"
#include <android/bitmap.h>
#include "vinmeasure.h"

/**
 * 在子线程里进行图片处理
 * @param args
 * @return
 */

#if 0
extern "C"
JNIEXPORT void JNICALL
Java_com_photo_photohandle_FirstActivity_setImageResult(JNIEnv *env, jobject thiz, jobject bitmap,jobject config) {
    // 处理图片的逻辑
    LOGE("==================java端调用了此方法,传递过来了bitmap");

//    env->GetJavaVM(&javaVM);
//
//    //c++在这里做图片处理逻辑，处理完成之后通过最下面的方法把bitmap传给Java端
//    Mat rgbImg;// = imread("/storage/emulated/0/PDAPhoto/1.jpeg");
    Mat vinImg;
//    bitmap2Mat(env, bitmap, &rgbImg, false);
//    cvtColor(rgbImg, rgbImg, CV_BGR2RGB);
//    LOGE("w = %d h = %d\n", rgbImg.cols, rgbImg.rows);
//
//    //VIN码测量处理
//    VinMeasureProcess(rgbImg, &vinImg);
//    cvtColor(vinImg, vinImg, CV_BGR2RGB);

    //mat2bitmap
    jobject result_bitmap = createBitmap(env, vinImg, config);
    jclass clazz = env->GetObjectClass(thiz);
    jmethodID methodId = env->GetMethodID(clazz, "onReceiveNativeBitmap",
                                          "(Landroid/graphics/Bitmap;)V");
}
#endif


void *process_bitmap(void *args) {
    PhotoHandler *photoHandler = static_cast<PhotoHandler *>(args);
    // 子线程里获取到bitmap
    //LOGE("子线程里获取到bitmap");
    photoHandler->startProcessPhoto();
    return  NULL;
};

void PhotoHandler::startProcessPhoto() {

    JNIEnv *jniEnv = NULL;
    javaVm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);
    if (jniEnv != JNI_OK) {
        return;
    }
    javaVm->AttachCurrentThread(&jniEnv, NULL);

    // test
//    char *msg = "error";
//    helper->onError(msg,101);

    // test end

    //c++在这里做图片处理逻辑，处理完成之后通过最下面的方法把bitmap传给Java端
    Mat rgbImg;// = imread("/storage/emulated/0/PDAPhoto/1.jpeg");
    Mat vinImg;
    jint code;
    bitmap2Mat(jniEnv, javaBitmap, &rgbImg, false);
    cvtColor(rgbImg, rgbImg, CV_BGR2RGB);
    LOGE("w = %d h = %d\n", rgbImg.cols, rgbImg.rows);

    //VIN码测量处理
    //VinMeasureProcess(rgbImg, &vinImg, code);
    VinMeasureProcess(rgbImg, &vinImg);
    cvtColor(vinImg, vinImg, CV_BGR2RGB);

    //mat2bitmap
    jobject result_bitmap = createBitmap(jniEnv, vinImg, bitmapConfig);

    jclass clazz = jniEnv->GetObjectClass(classObj);
    jmethodID methodId = jniEnv->GetMethodID(clazz, "onReceiveNativeBitmap",
                                          "(Landroid/graphics/Bitmap;I)V");
    jniEnv->CallVoidMethod(classObj,methodId,result_bitmap,code);
    //jniEnv->CallVoidMethod(classObj,methodId,result_bitmap);
    LOGE("code = %d",code);
    javaVm->DetachCurrentThread();
}


PhotoHandler::PhotoHandler(jobject jBitmap,JavaVM *vm,JavaCallbackHelper *javaCallbackHelper,jobject bitmapconfig,jobject classObj) {
    // 使用任意方式获取
    this->javaBitmap = jBitmap;
    this->javaVm = vm;
    this->helper = javaCallbackHelper;
    this->bitmapConfig = bitmapconfig;
    this->classObj = classObj;
}

PhotoHandler::~PhotoHandler() {
    // 析构函数做释放操作
}

/**
 * 开启一个线程，开始处理图片
 */
void PhotoHandler::processPhoto() {
    // 开启一个子线程进行图片处理
    //LOGE("开启一个子线程进行图片处理");
    pthread_create(&pid_img, NULL, process_bitmap, this);
}