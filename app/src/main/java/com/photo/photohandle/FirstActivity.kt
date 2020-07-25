package com.photo.photohandle

import android.app.Activity
import android.content.Intent
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Matrix
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
        path?.let {
            val bitmap = BitmapFactory.decodeFile(path)
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
           // Thread(Runnable { setImageResult(bitmap, Bitmap.Config.ARGB_8888) }).start()
            setImageResult(bitmap, Bitmap.Config.ARGB_8888)

        }
    }

    override fun onDestroy() {
        super.onDestroy()
        pageDestroy = true
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (resultCode != Activity.RESULT_OK) {
            return
        }
        if (requestCode == MainActivity.REQUEST_CODE && data != null) {
            val path = data.getStringExtra(MainActivity.IMG_PATH)

            path?.let {
                val bitmap = BitmapFactory.decodeFile(path)
                iv.setImageBitmap(bitmap)

                // 拍照保存成功，传给native方法处理
                // 同步获取结果
                //  val handlePhoto = JniUtil.handlePhoto("我是本地图片.jpeg")
                //  tvDesc.text = handlePhoto.toString()
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
    fun onPhotoProcessError(){
        runOnUiThread {
            println("=========================图片处理错误")
        }
    }
}