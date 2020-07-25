package com.photo.photohandle.utils

import com.photo.photohandle.bean.HandleResult

/**
 *  @author lxy
 *  @Description 用来和cpp交互的专用类
 */
object JniUtil {

    // 动态注册的方法
    external fun intFromJni(): Int

    // 同步处理图片
    external fun handlePhoto(imgPath: String): HandleResult
}