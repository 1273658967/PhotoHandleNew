# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)


#cwj添加--
##设置cpp文件的编译参数
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fexceptions -frtti -fopenmp")

##添加编译参数，添加宏定义
add_definitions(-DSEETA_NUM_THREADS=8)
add_definitions(-DDLIB_NO_GUI_SUPPORT)
add_definitions(-DDLIB_USE_BLAS_STR=ON)
add_definitions(-DDLIB_USE_BLAS=ON)
#--结束

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#cwj添加--
#set(libs "${CMAKE_SOURCE_DIR}/src/main/jniLibs")
#--结束

# 引入文件夹
include_directories(include)

#cwj添加--
#设置JNI文件夹目录
set(libs "${CMAKE_SOURCE_DIR}/src/main/jniLibs")

#配置头文件的搜索路径 #包含.h文件include目录
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp)
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include/opencv)
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include/opencv2)
#--结束

# 添加依赖 opencv.so 库
add_library(opencv_java3 SHARED IMPORTED)
SET_TARGET_PROPERTIES(opencv_java3 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libopencv_java3.so)

#cwj添加--
#动态--
add_library(vinmeasure SHARED IMPORTED )
set_target_properties(vinmeasure PROPERTIES
        IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libvinmeasure.so")
add_library(equalscaleimg SHARED IMPORTED )
set_target_properties(equalscaleimg PROPERTIES
        IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libequalscaleimg.so")
add_library(findcorners SHARED IMPORTED )
set_target_properties(findcorners PROPERTIES
        IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libfindcorners.so")
add_library(undistortimg SHARED IMPORTED )
set_target_properties(undistortimg PROPERTIES
        IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libundistortimg.so")
#--结束


## 找到所有的源文件
file(GLOB source_path ${CMAKE_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/*.h)

#cwj添加--
#??c++代码源文件
set(src_files
        src/main/cpp/equalscaleimg.cpp
        src/main/cpp/findcorners.cpp
        src/main/cpp/vinmeasure.cpp
        src/main/cpp/undistortimg.cpp
        )
#--结束

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${source_path})

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

# 在ndk中查找jnigraphics库 取别名jnigraphics-lib
# jnigraphics包含图形操作的库
find_library(jnigraphics-lib jnigraphics)

target_link_libraries( # Specifies the target library.
        native-lib
        opencv_java3
        ${jnigraphics-lib}

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})