//
// Created by eg on 2020/7/25.
//

#include "javaCallbackHelper.h"
#include "log.h"


JavaCallbackHelper::JavaCallbackHelper(JavaVM *vm,JNIEnv *env, jobject clazz) {
    this->vm = vm;
    this->env = env;
    this->classInstance = clazz;

    this->errorCallback = env->GetMethodID(env->GetObjectClass(clazz), "onPhotoProcessError",
                                               "()V");
    this->finishCallback = env->GetMethodID(env->GetObjectClass(clazz), "onReceiveNativeBitmap",
                                              "(Landroid/graphics/Bitmap;)V");
}

JavaCallbackHelper::~JavaCallbackHelper() {

}

void JavaCallbackHelper::onError(char *msg, int code) {
    JNIEnv *jniEnv = NULL;
    vm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);
    vm->AttachCurrentThread(&jniEnv, NULL);
    jniEnv->CallVoidMethod(classInstance, errorCallback, code, jniEnv->NewStringUTF(msg));
    vm->DetachCurrentThread();
}

void JavaCallbackHelper::onFinish(jobject bitmap) {
    JNIEnv *jniEnv = NULL;
    vm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);
    if (jniEnv != JNI_OK) {
        return;
    }
    vm->AttachCurrentThread(&jniEnv, NULL);
    jniEnv->CallVoidMethod(classInstance, finishCallback);
    vm->DetachCurrentThread();

}