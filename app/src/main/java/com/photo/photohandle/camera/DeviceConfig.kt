package com.photo.photohandle.camera

/**
 *  @author lxy
 *  @Description 配置拍摄照片的大小
 */
object DeviceConfig {
    // PDA预览分辨率
    const val PRE_PIX_WIDTH = 1600//1280
    const val PRE_PIX_HEIGHT = 1200//960
    // PDA拍摄照片的大小
    const val PIX_WIDTH = 2048
    const val PIX_HEIGHT = 1536

    // 拍照后保存路径
    const val ORIGIN_DIR = "VINTempDir"

    // 图片处理之后保存路径
    const val SAVE_DIR = "VIN"

    // 测试环境图片上传服务器地址
    const val SERVER_URL_DEV = "http://t.xinhuo.com/index.php/Api/Pic/uploadPic"

    // 正式环境图片上传服务器地址
    const val SERVER_URL_PRO = "http://t.xinhuo.com/index.php/Api/Pic/uploadPic"

}