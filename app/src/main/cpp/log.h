//
// Created by lxy on 2020/7/24.
//

#ifndef PHOTOHANDLE_LOG_H
#define PHOTOHANDLE_LOG_H

#include <jni.h>
#include <iostream>
#include <android/log.h>

#define TAG "jni_tag"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)



#endif //PHOTOHANDLE_LOG_H
