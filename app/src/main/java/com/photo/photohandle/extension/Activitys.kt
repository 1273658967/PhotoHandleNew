package com.photo.photohandle.extension

import android.view.View

/**
 *  @author lxy
 *  @Description activity相关的扩展方法
 */
// class Activitys {}

// inline fun AppCompatActivity?.startActivityForResult()


fun View?.show(show: Boolean) {
    this?.let {
        if (show) it.visibility = View.VISIBLE else it.visibility = View.GONE
    }
}
