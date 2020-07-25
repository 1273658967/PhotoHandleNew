//
// Created by eg on 2020/7/25.
//

#ifndef PHOTOHANDLE_JAVACALLBACKHELPER_H
#define PHOTOHANDLE_JAVACALLBACKHELPER_H

#include <jni.h>

class JavaCallbackHelper {
public:
    // JNIEnv 是线程私有的，因此需要JavaVM来进行跨线程操作
    JavaCallbackHelper(JavaVM *vm, JNIEnv *env, jobject classObj);

    ~JavaCallbackHelper();

    // 处理报错
    void onError(char *msg, int code);

    // 图片处理完成
    void onFinish(jobject bitmap);

private:
    JavaVM *vm;
    JNIEnv *env;
    jobject classInstance;
    jmethodID errorCallback;
    jmethodID finishCallback;
};


#endif //PHOTOHANDLE_JAVACALLBACKHELPER_H
