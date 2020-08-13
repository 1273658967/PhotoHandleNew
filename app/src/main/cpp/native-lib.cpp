#include <jni.h>
#include <string>
#include <iostream>
#include <android/log.h>
#include "bitmap-util.h"
#include "photoHandler.h"
using namespace std;
using namespace cv;
#define TAG "jni_tag"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)


JavaVM *mVm = NULL;

const char *className = "com/photo/photohandle/utils/JniUtil";

/**
 * 在此方法里动态注册java层方法
 * @param vm
 * @param re
 * @return
 */
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *re) {
    mVm = vm;
    return JNI_VERSION_1_6;
}

 //这里是c++代码，拍照完成后，会调用这个方法把bitmap传进来
extern "C"
JNIEXPORT void JNICALL
Java_com_photo_photohandle_FirstActivity_setImageResult(JNIEnv *env, jobject thiz, jobject bitmap,jobject config) {
    // 处理图片的逻辑
    LOGE("==================java端调用了此方法,传递过来了bitmap====");

    // 把bitmap放到单独的图片处理类里进行处理
    JavaCallbackHelper *helper = new JavaCallbackHelper(mVm,env,env->NewGlobalRef(thiz));
    PhotoHandler *photoHandler = new PhotoHandler(env->NewGlobalRef(bitmap),mVm,helper, env->NewGlobalRef(config),env->NewGlobalRef(thiz));
    // 调用处理图片的方法，开始处理逻辑
    photoHandler->processPhoto();

}