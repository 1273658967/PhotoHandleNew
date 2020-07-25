package com.photo.photohandle.bean

/**
 *  @author lxy
 *  @Description 图片处理结果类
 */
data class HandleResult(
    val imgPath: String,
    val resultCode: Int,
    val yaw: Double,
    val pitch: Double,
    val roll: Double
)