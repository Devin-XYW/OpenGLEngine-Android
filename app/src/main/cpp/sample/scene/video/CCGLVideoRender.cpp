//
// Created by xuwen on 2022-07-22.
//

#include <CCGLCamera.h>
#include "CCGLVideoRender.h"

CCGLVideoRender::CCGLVideoRender() {

}

CCGLVideoRender::~CCGLVideoRender() {
    CCGLSafePtrDelete(m_pOpenGLShader);

    glDeleteTextures(3,m_textures);

    if(NULL != mBufferYUV420p){
        free(mBufferYUV420p);
        mBufferYUV420p=NULL;
    }
}

void CCGLVideoRender::InitRenderResources(AAssetManager *pManager) {

    m_pAssetManager = pManager;

    //创建3个texture纹理
    glGenTextures(3,m_textures);

    //创建Shader类
    m_pOpenGLShader = new CCOpenGLShader();

    if(m_pAssetManager != NULL){
        //加载assets目录下的shader文件
        loadShaderResources(m_pAssetManager);
    }

}

void CCGLVideoRender::loadShaderResources(AAssetManager *pManager) {

    //从assets目录下读取shader文件:顶点着色器 和 片段着色器
    m_pOpenGLShader->InitShaderFromFile(pManager
                                        ,"shader/yuvplay/yuvplayv.glsl"
                                        ,"shader/yuvplay/yuvplayf.glsl");
}

void CCGLVideoRender::Render(CCGLCamera* pCamera) {
    //清屏
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(m_bUpdateData == false){
        return;
    }

    //当前渲染图像顶点坐标（全屏）和纹理坐标
    static CCVertex triangleVert[] = {
            {-1, 1,  1,     0,0},
            {-1, -1,  1,    0,1},
            {1,  1,  1,     1,0},
            {1,  -1,  1,    1,1},
    };

    //设置当前模板矩阵：向z轴负方向移动-3
    glm::mat4x4  objectMat = glm::mat4x4(1.0);
    glm::mat4x4  objectTransMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5));
    objectMat = objectMat * objectTransMat;
    //从当前Camera摄像头获取到当前的观察矩阵和投影矩阵
    //这里要注意顺序是从右往左进行计算
    objectMat = pCamera->projectionMatrix * pCamera->viewMatrix * objectMat;

    //开始绑定shader
    m_pOpenGLShader->Bind();

    //向shader中"uni_mat"变量传递变换矩阵
    m_pOpenGLShader->SetUniformValue("uni_mat",objectMat);

    //开启顶点坐标变量"attr_position"，并传递顶点坐标信息
    m_pOpenGLShader->EnableAttributeArray("attr_position");
    m_pOpenGLShader->SetAttributeBuffer("attr_position",GL_FLOAT,triangleVert,3, sizeof(CCVertex));

    //开启纹理坐标变量"attr_uv"，并传递纹理坐标信息
    m_pOpenGLShader->EnableAttributeArray("attr_uv");
    m_pOpenGLShader->SetAttributeBuffer("attr_uv",GL_FLOAT,&triangleVert[0].u,2,sizeof(CCVertex));

    //传递Y分量数据texture纹理数据
    m_pOpenGLShader->SetUniformValue("uni_textureY",0);
    glActiveTexture(GL_TEXTURE0);//设置当前第一个纹理缓存区活跃状态
    glBindTexture(GL_TEXTURE_2D,m_textures[0]);//绑定第一个纹理，后续操作都在其之上
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,mVideoWidth,mVideoHeight
                 ,0,GL_LUMINANCE, GL_UNSIGNED_BYTE,mBufferYUV420p);//设置Y纹理坐标数据
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //传递U分量数据texture纹理数据
    m_pOpenGLShader->SetUniformValue("uni_textureU",1);
    glActiveTexture(GL_TEXTURE1);//设置当前第二个纹理缓存区活跃状态
    glBindTexture(GL_TEXTURE_2D,m_textures[1]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE
                 ,mVideoWidth/2, mVideoHeight/2, 0
                 , GL_LUMINANCE, GL_UNSIGNED_BYTE
                 , (char*)(mBufferYUV420p+mYFrameLength));//设置U分量数据坐标，最后一个参数为U分量在数组中的起始位置
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //传递V分量数据texture纹理数据
    m_pOpenGLShader->SetUniformValue("uni_textureV",2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_textures[2]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, mVideoWidth/2, mVideoHeight/2, 0
                 , GL_LUMINANCE, GL_UNSIGNED_BYTE
                 , (char*)(mBufferYUV420p+mYFrameLength+mVFrameLength));//设置V纹理数据
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //开始绘图
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);

    m_pOpenGLShader->DisableAttributeArray("attr_position");
    m_pOpenGLShader->DisableAttributeArray("attr_uv");

    m_pOpenGLShader->Release();

    return;
}

void CCGLVideoRender::UpdateYUVData(YUVData_Frame *yuvFrame) {
    if(yuvFrame == NULL ){
        return;
    }

    if(mVideoHeight != yuvFrame->height || mVideoWidth != yuvFrame->width){

        if(NULL != mBufferYUV420p)
        {
            free(mBufferYUV420p);
            mBufferYUV420p=NULL;
        }
    }

    mVideoWidth =  yuvFrame->width;
    mVideoHeight = yuvFrame->height;

    mYFrameLength = yuvFrame->luma.length;
    mUFrameLength = yuvFrame->chromaB.length;
    mVFrameLength = yuvFrame->chromaR.length;

    //生气内存存放一帧yuv图像大小，大小为分辨率的1.5倍（相比RGB是3倍）
    int nLen = mYFrameLength + mUFrameLength + mVFrameLength;
    if(NULL == mBufferYUV420p){
        mBufferYUV420p = (unsigned char*) malloc(nLen);
    }
    //拷贝Y分量数据
    memcpy(mBufferYUV420p,yuvFrame->luma.dataBuffer,mYFrameLength);
    //拷贝U分量数据
    memcpy(mBufferYUV420p+mYFrameLength,yuvFrame->chromaB.dataBuffer,mUFrameLength);
    //拷贝V分量数据
    memcpy(mBufferYUV420p+mYFrameLength+mUFrameLength,yuvFrame->chromaR.dataBuffer,mVFrameLength);

    m_bUpdateData = true;
}

