//
// Created by xuwen on 2022-07-22.
//

#ifndef OPENGLENGINE_CCGLVIDEORENDER_H
#define OPENGLENGINE_CCGLVIDEORENDER_H

#include "CCOpenGLShader.h"
#include "CCNDKCommonDef.h"
#include "CCYUVDataDefine.h"
#include "CCGLCamera.h"

class CCGLVideoRender {
public:
    CCGLVideoRender();
    ~CCGLVideoRender();

    //初始化：加载shader等
    void  InitRenderResources(AAssetManager *pManager);
    //进行Render相关配置
    void Render(CCGLCamera* camera);
    //传递YUV帧数据并进行渲染
    void UpdateYUVData(YUVData_Frame *frame);

private:
    /**
     * 从assets目录下读取当前shader文件
     */
    void loadShaderResources(AAssetManager *pManager);

private:
    AAssetManager* m_pAssetManager;//assets目录Manager
    CCOpenGLShader* m_pOpenGLShader;//当前OpenGLShader方法类

    bool m_bUpdateData = false;//标志YUV数据是否加载完成

    GLuint m_textures[3];//渲染YUV数据，需要3个texture

    int mVideoWidth = 0;//视频分辨率宽
    int mVideoHeight = 0;//视频分辨率高

    int mYFrameLength = 0;
    int mUFrameLength = 0;
    int mVFrameLength = 0;

    //YUV数据
    unsigned char* mBufferYUV420p = NULL;

    struct CCVertex{
        float x,y,z;
        float u,v;
    };

};


#endif //OPENGLENGINE_CCGLVIDEORENDER_H
