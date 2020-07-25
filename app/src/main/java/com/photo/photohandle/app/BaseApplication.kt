package com.photo.photohandle.app

import android.app.Application

class BaseApplication : Application() {
    override fun onCreate() {
        super.onCreate()
        System.loadLibrary("native-lib")
    }
}