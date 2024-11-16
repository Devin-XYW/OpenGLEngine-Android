package com.xuwen.openglengine.type

object IMyNativeRendererType {

    const val RENDER_TYPE: String = "RENDER_TYPE"

    const val SAMPLE_TYPE = 100
    //创建绘制三角形
    const val SAMPLE_TYPE_2D_TEXTURE = SAMPLE_TYPE
    //创建3D特效
    const val SAMPLE_TYPE_3D_CAMERA = SAMPLE_TYPE + 1
    //创建纹理缩放、旋转等操作
    const val SAMPLE_TYPE_TEXTURE_SCALE = SAMPLE_TYPE + 2
    //抖音滤镜
    const val SAMPLE_TYPE_CAMERA = SAMPLE_TYPE + 3
    //转场动画
    const val SAMPLE_TYPE_TRANSITION = SAMPLE_TYPE + 4
    //绿幕抠图
    const val SAMPLE_TYPE_GREENSCREEN = SAMPLE_TYPE + 5
    //渲染3D模型
    const val SAMPLE_TYPE_3D_MODEL = SAMPLE_TYPE + 6
    //渲染3D动画模型
    const val SAMPLE_TYPE_3D_ANIM_MODEL = SAMPLE_TYPE + 7
    //渲染3D动画模型
    const val SAMPLE_TYPE_3D_ANIM_MODEL_1 = SAMPLE_TYPE + 8

    const val SAMPLE_TYPE_SET_TOUCH_LOC = SAMPLE_TYPE + 999
    const val SAMPLE_TYPE_SET_GRAVITY_XY = SAMPLE_TYPE + 1000
}