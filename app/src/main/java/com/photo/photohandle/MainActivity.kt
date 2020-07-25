package com.photo.photohandle

import android.content.Intent
import android.graphics.ImageFormat
import android.hardware.Camera
import android.os.Bundle
import android.os.Environment
import android.view.SurfaceHolder
import android.view.WindowManager
import androidx.appcompat.app.AppCompatActivity
import com.photo.photohandle.bean.SupportSize
import com.photo.photohandle.camera.DeviceConfig
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File
import java.io.FileOutputStream


class MainActivity : AppCompatActivity(), SurfaceHolder.Callback {

    private var mCamera: Camera? = null
    private val mHolder by lazy { mSurfaceView.holder }
    private val mPictureCallback: Camera.PictureCallback by lazy {
        Camera.PictureCallback { data, _ ->
            val path = Environment.getExternalStorageDirectory().toString()
            val file = File(path, DeviceConfig.ORIGIN_DIR)
            if (!file.exists()) {
                file.mkdirs()
            }
            val tepFile = File(file, "${System.currentTimeMillis()}.jpeg")
            try {
                val fileOutputStream = FileOutputStream(tepFile)
                fileOutputStream.write(data)
                fileOutputStream.close()

                //跳转页面
                val intent = Intent(this, FirstActivity::class.java)
                intent.putExtra(IMG_PATH, tepFile.path)
                startActivity(intent)


            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }



    private val mSupportSizeList = mutableListOf<SupportSize>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        window.setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        )
        setContentView(R.layout.activity_main)
        mHolder.addCallback(this)
        // 开发/生产环境的tag
        val envTag = BuildConfig.envTag

        if (mCamera == null) {
            mCamera = getCamera()
        }

        mCamera?.let {
            val parameters = it.parameters
            val list = parameters.supportedPictureSizes
            mSupportSizeList.clear()
            list.forEach {
                val item = SupportSize(it.width, it.height)
                mSupportSizeList.add(item)
            }
        }

        btnTakePhoto.setOnClickListener {
            takePhoto()
        }

        mSurfaceView.setOnClickListener {
            mCamera?.autoFocus(null)
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        val REQUEST_CODE = 1001
        //val IMG_PATH = "img_path"
        val IMG_PATH = "img_path"
    }

    override fun onResume() {
        super.onResume()
        if (mCamera == null) {
            mCamera = getCamera()
            if (mHolder != null) {
                startPreview(mCamera, mHolder)
            }
        }
    }

    override fun onPause() {
        super.onPause()
        releaseCamera()
    }

    private fun getCamera(): Camera? {
        var camera: Camera? = null
        try {
            camera = Camera.open()
        } catch (e: Exception) {
            e.printStackTrace()
        }
        return camera
    }

    private fun releaseCamera() {
        mCamera?.let {
            it.setPreviewCallback(null)
            it.stopPreview()
            it.release()
            mCamera = null
        }
    }

    private fun startPreview(camera: Camera?, holder: SurfaceHolder) {
        camera ?: return
        try {
            camera.setPreviewDisplay(holder)
            // 固定横屏显示，预览不用加旋转角度
            camera.setDisplayOrientation(90)

            resetSurfaceParams(camera)
            camera.startPreview()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    /**
     * 重置surfaceView的大小以解决预览变形问题
     */
    fun resetSurfaceParams(camera: Camera) {
        val _parameters = camera.parameters
        _parameters.setPreviewSize(DeviceConfig.PRE_PIX_WIDTH, DeviceConfig.PRE_PIX_HEIGHT)
        camera.parameters = _parameters
    }

    private fun takePhoto() {
        mCamera?.let {
            val parameters = it.parameters
            parameters.pictureFormat = ImageFormat.JPEG
            parameters.setPictureSize(DeviceConfig.PIX_WIDTH, DeviceConfig.PIX_HEIGHT)
            parameters.flashMode = Camera.Parameters.FOCUS_MODE_AUTO
            it.parameters = parameters
            it.autoFocus { success, _ ->
                // 对焦成功
                if (true) {
                    mCamera?.takePicture(null, null, mPictureCallback)
                }
            }
        }
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        mCamera?.stopPreview()
        mCamera?.let {
            startPreview(it, mHolder)
        }
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        releaseCamera()
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        holder?.let {
            startPreview(mCamera, holder)
        }
    }
}
