//
// Created by xuwen on 2022-08-06.
//

#ifndef OPENGLENGINE_GLBASESAMPLE_H
#define OPENGLENGINE_GLBASESAMPLE_H

#include "GLBaseSampleType.h"
#include "CCNDKCommonDef.h"
#include "CCYUVDataDefine.h"
#include "CCOpenGLFBO.h"
#include "CCOpenGLTexture.h"
#include "CCOpenGLBuffer.h"
#include "CCModelLoader.h"
#include "CCTextureRender.h"

class GLBaseSample{
public:
    GLBaseSample(){}
    virtual ~GLBaseSample(){}

    //初始化当前AssetsManager
    virtual void SetupAssetManager(AAssetManager *assetManager,std::string path) = 0;

    virtual void InitGL() = 0;//SurfaceView创建时进行初始化
    virtual void PaintGL() = 0;//渲染相关图像
    virtual void ResizeGL(int width,int height) = 0;//SurfaceView画面改变时调用
    virtual void Release() = 0;//释放资源

    //传递当前用户在屏幕上进行缩放、旋转等信息
    virtual void updateTransformMatrix(float rotateX, float rotateY
                                       , float scaleX, float scaleY){};

    //上层传递当前在屏幕上触碰位置
    virtual void SetTouchLocation(float x, float y){}
    //上层传递当前XY位置
    virtual void SetGravityXY(float x, float y){}

    virtual void RenderVideo(YUVData_Frame *frame){};//将yuv数据传递进行视频渲染
    virtual void RecordVideo(bool state){};//记录Video数据，封装成MP4

    virtual void UpdateTrackingInfo(glm::vec2 pos,float scale){};

};

#endif //OPENGLENGINE_GLBASESAMPLE_H
