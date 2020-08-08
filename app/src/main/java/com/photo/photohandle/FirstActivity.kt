package com.photo.photohandle

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Matrix
import android.graphics.drawable.BitmapDrawable
import android.os.Bundle
import android.os.Environment
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.rxLifeScope
import com.photo.photohandle.bean.HandleResult
import com.photo.photohandle.camera.DeviceConfig
import com.photo.photohandle.extension.show
import kotlinx.android.synthetic.main.activity_first.*
import rxhttp.RxHttp
import rxhttp.toStr
import java.io.File
import java.io.FileOutputStream


/**
 *  @author lxy
 *  @Description
 */
class FirstActivity : AppCompatActivity() {
    private var pageDestroy = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_first)
        val path = intent.getStringExtra(MainActivity.IMG_PATH)
        path ?: return
        iv.post {
            val imageViewHeight = iv.height
            val imageViewWidth = iv.width
            val bitmap = getBitmapFromFile(path, imageViewWidth, imageViewHeight)
            val m = Matrix()
            m.setRotate(90f, bitmap.width / 2f, bitmap.height / 2f)
            try {
                val temBitmap =
                    Bitmap.createBitmap(bitmap, 0, 0, bitmap.width, bitmap.height, m, true)
                iv.setImageBitmap(temBitmap)
            } catch (e: Exception) {
                e.printStackTrace()
            }
            // 调用c++端方法，把bitmap传过去
            LoadingLayout.show(true)
            setImageResult(bitmap, Bitmap.Config.ARGB_8888)
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        pageDestroy = true
        iv?.let {
            val bitmapDrawable = it.drawable as? BitmapDrawable
            bitmapDrawable ?: return
            val bitmap: Bitmap? = bitmapDrawable.bitmap
            if (bitmap != null && !bitmap.isRecycled) {
                bitmap.recycle()
            }
        }
    }

    /**
     * 接收到native处理图片后的回调
     */
    fun onReceiveNativeImg(bean: HandleResult) {
        runOnUiThread {
            tvDesc.text = "子线程收到处理的信息${bean.toString()}"
        }
    }

    /**
     * 拍照后，java端把图片传给c++，c++处理完之后调用下面的onReceiveNativeBitmap方法把bitmap传回来
     */
    external fun setImageResult(bitmap: Bitmap, config: Bitmap.Config)

    /**
     * 接收到native处理图片后的回调
     */
    fun onReceiveNativeBitmap(bitmap: Bitmap?) {
        if (pageDestroy) return
        LoadingLayout.show(bitmap != null)
        runOnUiThread {
            bitmap?.let {
                iv.setImageBitmap(it)
            }
        }

        // todo 上传该图片到服务器
        bitmap?.let {
            upload(it)
        }
    }


    //文件上传---测试代码
    private fun upload(bitmap: Bitmap) = rxLifeScope.launch({

        val path = Environment.getExternalStorageDirectory().toString()
        val file = File(path, DeviceConfig.SAVE_DIR)
        if (!file.exists()) file.mkdirs()
        val uploadFile = File(file, "${System.currentTimeMillis()}.jpeg")
        if (uploadFile.exists()) uploadFile.delete()
        try {
            val out = FileOutputStream(uploadFile)
            bitmap.compress(Bitmap.CompressFormat.JPEG, 100, out)
            out.flush()
            out.close()

            var serverUrl = ""
            val envTag = BuildConfig.envTag
            if (envTag.equals("pro")) {
                serverUrl = DeviceConfig.SERVER_URL_PRO
            } else {
                serverUrl = DeviceConfig.SERVER_URL_DEV
            }
            val result = RxHttp.postForm(serverUrl)
                .addFile("uploaded_file", uploadFile)
                .toStr()
                .await()
            tvDesc.text = result
            // 图片上传成功之后删除本地图片
//            val orgFile = File(path, DeviceConfig.ORIGIN_DIR)
//            deleteFile(orgFile)

        } catch (e: Exception) {
            e.printStackTrace()
        } finally {
            LoadingLayout.show(false)
        }
    }, {
        //失败回调
        tvDesc.text = "上传失败,请稍后再试!"
        LoadingLayout.show(false)
    })

    /**
     * 删除PDAPhoto目录下所有文件
     */
    private fun deleteFile(file: File?) {
        file ?: return
        if (file.isFile) {
            file.delete()
        } else {
            val list = file.list()
            list?.forEach {
                val cf = File(file.absolutePath, it)
                deleteFile(cf)
            }
            file.delete()
        }
    }

    // native图片处理错误的回调
    fun onPhotoProcessError() {
        runOnUiThread {
            println("=========================图片处理错误")
        }
    }

    /**
     * 图片路径、期望的imageview的宽/高
     */
    private fun getBitmapFromFile(path: String, reqWidth: Int, reqHeight: Int): Bitmap {
        val options = BitmapFactory.Options()
        options.inJustDecodeBounds = true
        BitmapFactory.decodeFile(path, options)
        options.inSampleSize = calculateSampleSize(options, reqWidth, reqHeight)
        options.inJustDecodeBounds = false
        return BitmapFactory.decodeFile(path, options)
    }


    /**
     * 根据要显示的图片的实际大小计算压缩采样率
     * 注意：这里只是对显示在设备上的图片进行压缩，不影响存储在设备硬盘的实际图片
     */
    private fun calculateSampleSize(
        options: BitmapFactory.Options,
        reqWidth: Int,
        reqHeight: Int
    ): Int {
        val height = options.outHeight
        val width = options.outWidth
        var simpleSize = 1
        if (height > reqHeight || width > reqWidth) {
            val halfHeight = height / 2
            val halfWidth = width / 2
            while ((halfHeight / simpleSize) >= reqHeight &&
                (halfWidth / simpleSize) >= reqWidth
            ) {
                simpleSize *= 2
            }
        }
        return simpleSize
    }

}