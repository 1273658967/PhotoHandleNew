//
// Created by lxy on 2020/7/24.
//

#ifndef PHOTOHANDLE_PHOTOHANDLER_H
#define PHOTOHANDLE_PHOTOHANDLER_H

#include <jni.h>
#include <opencv2/opencv.hpp>
#include "javaCallbackHelper.h"

class PhotoHandler{
public:
    // 通过构造方法传进来
    PhotoHandler(jobject jBitmap,JavaVM *vm, JavaCallbackHelper *helper,jobject bitmapconfig,jobject clazz);

    ~PhotoHandler();

    // 进行图片处理的方法
    void processPhoto();

    pthread_t pid_img;

    // 这里就是java端传进来的bitmap
    jobject javaBitmap;
    JavaVM * javaVm = NULL;

    void startProcessPhoto();

    JavaCallbackHelper *helper = NULL;

    jobject bitmapConfig;

    jobject classObj;

private:
    // JavaVM *vm = NULL;

};




#endif //PHOTOHANDLE_PHOTOHANDLER_H
