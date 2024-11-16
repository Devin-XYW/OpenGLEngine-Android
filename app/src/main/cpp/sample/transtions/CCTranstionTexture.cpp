//
// Created by xuwen on 2022-08-22.
//

#include "CCTranstionTexture.h"

CCTranstionTexture::CCTranstionTexture() {

    m_pVAO = new CCOpenGLVAO();
    m_pVBO = new CCOpenGLBuffer(CCOpenGLBuffer::VertexBuffer,CCOpenGLBuffer::StaticDraw);
    m_pEBO = new CCOpenGLBuffer(CCOpenGLBuffer::IndexBuffer,CCOpenGLBuffer::StaticDraw);
    m_pCamera = new CCGLCamera();//初始化摄像机

    m_pOpenGLShader = new CCOpenGLShader();
}

CCTranstionTexture::~CCTranstionTexture() {
    glDeleteTextures(1,&m_texId_1);
    glDeleteTextures(1,&m_texId_2);
    CCGLSafePtrDelete(m_pCamera);//释放摄像机
    CCGLSafePtrDelete(m_pVAO);
    CCGLSafePtrDelete(m_pVBO);
    CCGLSafePtrDelete(m_pEBO);

    CCGLSafePtrDelete(m_pOpenGLShader);
}

void CCTranstionTexture::SetupAssetManager(AAssetManager *assetManager, std::string path) {
    m_pAssetManager = assetManager;
    m_directoryPath = path;
}

void CCTranstionTexture::InitGL() {

    //首先进行清屏
    glClearColor(0.0,0.0,0.0,1.0);
    glClearDepthf(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //加载当前纹理图片及shader
    if(m_pAssetManager != NULL){
        //加载纹理图片
        loadTextureResource(m_pAssetManager);
        //加载shader程序
        loadShaderResource(m_pAssetManager);
    }

    //加载顶点、纹理等信息
    setupPlaneResource();
}

void CCTranstionTexture::loadTextureResource(AAssetManager *pManager) {
    //创建纹理图片
    CCOpenGLTexture glTexture;
    m_texId_1 = glTexture.CreateTextureFromFile(pManager,"texture/zhoushan1.png");
    m_texId_2 = glTexture.CreateTextureFromFile(pManager,"texture/zhoushan2.png");
    mVideoWidth = glTexture.getWidth();
    mVideoHeight = glTexture.getHeight();
}

void CCTranstionTexture::loadShaderResource(AAssetManager *pManager) {
    //加载编译顶点着色器和片段着色器
    m_pOpenGLShader->InitShaderFromFile(pManager,
                                        "shader/transition/transitionv.glsl",
                                        "shader/transition/transitionf_9.glsl");
}

void CCTranstionTexture::setupPlaneResource() {

    if(m_pOpenGLShader == NULL){
        return;
    }

    //贴图的顶点坐标和纹理坐标
    const CCFloat5 planeVertexs[] = {
            {  -1.0f,  0.5f, 0.0f,  1.0, 1.0 },
            {  -1.0f, -0.5f, 0.0f,  1.0, 0.0 },
            {  1.0f, -0.5f, 0.0f, 0.0, 0.0 },
            {  1.0f,  0.5f, 0.0f,  0.0, 1.0 },
    };

    //贴图索引坐标
    const unsigned short planeIndexs[]= {
            0, 1, 2,  0, 2, 3
    };

    //创建并绑定VAO
    m_pVAO->Create();
    m_pVAO->Bind();

    //创建、绑定、初始化VBO数据
    m_pVBO->Create();
    m_pVBO->Bind();
    m_pVBO->SetBufferData(planeVertexs,sizeof(planeVertexs));

    //创建、绑定、初始化EBO数据
    m_pEBO->Create();
    m_pEBO->Bind();
    m_pEBO->SetBufferData(planeIndexs, sizeof(planeIndexs));

    //解释当前shader中相关的顶点坐标属性、纹理顶点属性
    int offset = 0;//顶点坐标偏移量
    //解释shader中的position属性，根据location设置的位置进行查找
    m_pOpenGLShader->SetAttributeBuffer(0,GL_FLOAT,(void *) offset,3, sizeof(CCFloat5));
    //开启position属性，默认是关闭的
    m_pOpenGLShader->EnableAttributeArray(0);

    offset += 3 * sizeof(float);//设置纹理顶点数据偏移量，数据开始位置

    //解释shader中的uv属性，根据location设置的位置进行查找
    m_pOpenGLShader->SetAttributeBuffer(1,GL_FLOAT,(void *) offset,2, sizeof(CCFloat5));
    //开启uv属性，默认是关闭的
    m_pOpenGLShader->EnableAttributeArray(1);

    m_pVAO->Release();
    m_pVBO->Release();
    m_pEBO->Release();
}

void CCTranstionTexture::PaintGL() {
    //清空颜色缓冲区和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(m_pOpenGLShader == NULL){
        return;
    }

    m_offset += 0.005f;
    if(m_offset > 1.0f){
        m_offset = 0.0f;
    }

    //初始化MVP矩阵
    glm::mat4x4 objectMat = glm::mat4x4(1.0);
    glm::mat4x4 objectTransMat = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, -2.0));
    objectMat = objectMat * objectTransMat;
    objectMat = m_pCamera->projectionMatrix * m_pCamera->viewMatrix * objectMat;

    m_pOpenGLShader->Bind();
    //将MVP矩阵传递给shader
    m_pOpenGLShader->SetUniformValue("u_mat",objectMat);

    m_pVAO->Bind();

    //启用纹理贴图1到shader中
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_texId_1);
    m_pOpenGLShader->SetUniformValue("texture_1",0);

    //启用纹理贴图2到shader中
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,m_texId_2);
    m_pOpenGLShader->SetUniformValue("texture_2",1);

    //向shader中“u_offset”变量传递偏移量值
    m_pOpenGLShader->SetUniformValue("u_offset",m_offset);

    //向shader中“textSize”传递当前纹理宽高值
    float width = (float)mVideoWidth + 0.0;
    float height = (float)mVideoHeight + 0.0;
    m_pOpenGLShader->SetUniformValue("textSize",glm::vec2(width,height));

    //开始绘制
    const short* indices = (const short *) 0;
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,indices);

    glBindTexture(GL_TEXTURE_2D,0);
    glBindTexture(GL_TEXTURE_2D,1);

    m_pVAO->Release();
    m_pOpenGLShader->Release();
}

void CCTranstionTexture::ResizeGL(int width, int height) {
    glViewport(0,0,width,height);
}

void CCTranstionTexture::Release() {
    glDeleteTextures(1,&m_texId_1);
    glDeleteTextures(1,&m_texId_2);
    CCGLSafePtrDelete(m_pCamera);//释放摄像机
    CCGLSafePtrDelete(m_pVAO);
    CCGLSafePtrDelete(m_pVBO);
    CCGLSafePtrDelete(m_pEBO);

    CCGLSafePtrDelete(m_pOpenGLShader);
}








