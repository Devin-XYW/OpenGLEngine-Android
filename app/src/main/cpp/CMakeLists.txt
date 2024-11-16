cmake_minimum_required(VERSION 3.10.2)

project("openglengine")

#设置三方库文件路径
set(GLM_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libglm)
set(LIBOPENCV_DIR ${CMAKE_SOURCE_DIR}/3rdparty/OpenCV-android-sdk)
set(STB_IMAGE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libstbImage)
set(LIBYUV_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libyuv)
set(FFMPEG_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libFFmpeg)
set(FAAC_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libfaac)
set(FACETRACKER_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libfacetracker)
set(ASSIMP_DIR ${CMAKE_SOURCE_DIR}/3rdparty/assimp)
set(MODEL_DIR ${CMAKE_SOURCE_DIR}/3rdparty/model)

#引入三方库头文件
include_directories(${GLM_DIR}/include)
include_directories(${LIBOPENCV_DIR}/sdk/native/jni/include)
include_directories(${STB_IMAGE_DIR}/include)
include_directories(${YUV_DIR}/${ANDROID_ABI}/include)
include_directories(${FFMPEG_DIR}/${ANDROID_ABI}/include)
INCLUDE_DIRECTORIES(${FAAC_DIR}/${ANDROID_ABI}/include)
INCLUDE_DIRECTORIES(${LIBYUV_DIR}/${ANDROID_ABI}/include)
INCLUDE_DIRECTORIES(${MODEL_DIR})

#引入FFmpeg静态库
INCLUDE_DIRECTORIES(${FFMPEG_DIR}/${ANDROID_ABI}/include)
LINK_DIRECTORIES(${FFMPEG_DIR}/${ANDROID_ABI}/lib)
SET(FFMPEG_LIB avformat avcodec avdevice avfilter avutil swresample swscale fdk-aac x264 z c m dl)

#引入faac静态库文件
INCLUDE_DIRECTORIES(${FAAC_DIR}/${ANDROID_ABI}/include)
LINK_DIRECTORIES(${FAAC_DIR}/${ANDROID_ABI}/lib)
SET(FAAC_LIB faac)

#引入YUV静态库文件
INCLUDE_DIRECTORIES(${LIBYUV_DIR}/${ANDROID_ABI}/include)
LINK_DIRECTORIES(${LIBYUV_DIR}/${ANDROID_ABI}/lib)
SET(LIBYUV_LIB yuv)

#引入FACETRACKER静态库文件
INCLUDE_DIRECTORIES(${FACETRACKER_DIR}/include)
LINK_DIRECTORIES(${FACETRACKER_DIR}/libs/${ANDROID_ABI})
SET(LIBFACETRACKER facetracker)

#引入assimp
INCLUDE_DIRECTORIES(${ASSIMP_DIR})
add_library(assimp SHARED IMPORTED )
set_target_properties(assimp
        PROPERTIES
        IMPORTED_LOCATION
        ${ASSIMP_DIR}/lib/${CMAKE_ANDROID_ARCH_ABI}/libassimp.so)

#引入本地头文件目录
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/util
        ${CMAKE_CURRENT_SOURCE_DIR}/util/common
        ${CMAKE_CURRENT_SOURCE_DIR}/util/image
        ${CMAKE_CURRENT_SOURCE_DIR}/util/opengl/shader
        ${CMAKE_CURRENT_SOURCE_DIR}/util/opengl/buffer
        ${CMAKE_CURRENT_SOURCE_DIR}/util/opengl/texture
        ${CMAKE_CURRENT_SOURCE_DIR}/util/opengl/vao

        ${CMAKE_CURRENT_SOURCE_DIR}/util/ffmpeg/videowriter
        ${CMAKE_CURRENT_SOURCE_DIR}/util/ffmpeg/videodecoder
        ${CMAKE_CURRENT_SOURCE_DIR}/util/ffmpeg/audiodecoder

        ${CMAKE_CURRENT_SOURCE_DIR}/sample
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/scene
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/scene/video
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/scene/camera
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/scene/primitives
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/scene/primitives/mesh
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/scene/primitives/cube
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/scene/primitives/sticker
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/scene/primitives/fbotexture
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/texture2d
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/camera
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/transtions
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/greenscreen
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/3DSample
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/3DAnimSample
        ${CMAKE_CURRENT_SOURCE_DIR}/sample/3DModel
)

#引入android相关模块、log、GLESv3、OPenGLES
SET(ANDROID_SYSLIBS android log GLESv3 OpenSLES)

# 引入opencv库
add_library(LIBOPENCV SHARED IMPORTED)
set_target_properties(LIBOPENCV PROPERTIES IMPORTED_LOCATION ${LIBOPENCV_DIR}/sdk/native/libs/${ANDROID_ABI}/libopencv_java4.so)

# 设置本地代码路径匹配
file(GLOB src-files

        ${CMAKE_SOURCE_DIR}/*.cpp

        ${CMAKE_SOURCE_DIR}/*/*.cpp

        ${CMAKE_SOURCE_DIR}/*/*/*.cpp

        ${CMAKE_SOURCE_DIR}/*/*/*.cpp

        ${CMAKE_SOURCE_DIR}/*/*/*/*.cpp

        ${CMAKE_SOURCE_DIR}/*/*/*/*/*.cpp

        ${CMAKE_SOURCE_DIR}/*/*/*/*/*/*.cpp

        ${CMAKE_SOURCE_DIR}/*/*/*/*/*/*/*.cpp

        ${CMAKE_SOURCE_DIR}/*/*/*/*/*/*/*/*.cpp

        ${CMAKE_SOURCE_DIR}/*/*/*/*/*/*/*/*/*.cpp

        ${CMAKE_SOURCE_DIR}/*/*/*/*/*/*/*/*/*/*.cpp

        ${CMAKE_SOURCE_DIR}/*/*/*/*/*/*/*/*/*/*/*.cpp)

add_library(
        openglengine

        SHARED

        ${src-files})

target_link_libraries(
        openglengine

        ${ANDROID_SYSLIBS}

        LIBOPENCV

        assimp

        ${LIBFACETRACKER}

        ${FFMPEG_LIB}

        ${FAAC_LIB}

        ${LIBYUV_LIB})