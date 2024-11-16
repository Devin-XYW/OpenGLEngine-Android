//
// Created by xuwen on 2022-08-08.
//

#ifndef OPENGLENGINE_CCGLCAMERA2D_H
#define OPENGLENGINE_CCGLCAMERA2D_H

#include "GLBaseSample.h"

class CCGLCamera2D: public GLBaseSample {

public:
    CCGLCamera2D();
    ~CCGLCamera2D();

    //初始化当前AssetsManger
    void SetupAssetManager(AAssetManager *assetManager,std::string path);

    void InitGL();//SurfaceView创建时进行初始化
    void PaintGL();//渲染相关图像
    void ResizeGL(int width,int height);//SurfaceView画面改变时调用
    void Release();

    void RenderVideo(YUVData_Frame *frame);//将yuv数据传递进行视频渲染

private:
    void loadShaderResource(AAssetManager *pManager);//加载Shader资源
    void setupPlaneResource();//加载顶点、纹理坐标等资源

private:

    std::string m_directoryPath;
    AAssetManager* m_pAssetManager;//当前asset文件目录

    CCGLCamera* m_pCamera;//3D场景中的摄像机观察者
    CCOpenGLVAO* m_pVAO;//声明VAO
    CCOpenGLBuffer* m_pVBO;//声明VBO
    CCOpenGLBuffer* m_pEBO;//声明EBO

    CCOpenGLShader* m_pOpenGLShader;//创建当前shader

    bool m_bUpdateData = false;//标志当前YUV数据是否加载完成

    GLuint m_textures[3];//渲染YUV数据，需要3个texture

    float m_nValue = 0.0f;
    int mVideoWidth = 0;//视频分辨率宽
    int mVideoHeight = 0;//视频分辨率高

    int mYFrameLength = 0;//Y分量长度
    int mUFrameLength = 0;//U分量长度
    int mVFrameLength = 0;//V分量长度

    //YUV数据
    unsigned char* mBufferYUV420p = NULL;

    struct CCVertex{
        float x,y,z;
        float u,v;
    };

};


#endif //OPENGLENGINE_CCGLCAMERA2D_H
