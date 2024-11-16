//
// Created by xuwen on 2022-08-07.
//

#include "CCSampleTexture2DScale.h"

#define MATH_PI 3.1415926535897932384626433832802

CCSampleTexture2DScale::CCSampleTexture2DScale() {

    m_AngleX = 0;
    m_AngleY = 0;
    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;
    m_Width = 0;
    m_Height = 0;

    m_pVAO = new CCOpenGLVAO();
    m_pVBO = new CCOpenGLBuffer(CCOpenGLBuffer::VertexBuffer,CCOpenGLBuffer::StaticDraw);
    m_pEBO = new CCOpenGLBuffer(CCOpenGLBuffer::IndexBuffer,CCOpenGLBuffer::StaticDraw);
    m_pCamera = new CCGLCamera();//初始化摄像机

    m_pOpenGLShader = new CCOpenGLShader();
}

CCSampleTexture2DScale::~CCSampleTexture2DScale() {
    glDeleteTextures(1,&m_texId);
    CCGLSafePtrDelete(m_pCamera);//释放摄像机
    CCGLSafePtrDelete(m_pVAO);
    CCGLSafePtrDelete(m_pVBO);
    CCGLSafePtrDelete(m_pEBO);

    CCGLSafePtrDelete(m_pOpenGLShader);
}

void CCSampleTexture2DScale::SetupAssetManager(AAssetManager *assetManager, std::string path) {
    m_pAssetManager = assetManager;
}

void CCSampleTexture2DScale::InitGL() {
    //清除屏幕
    glClearColor(0.0,0.0,0.0,1.0);
    glClearDepthf(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //加载当前纹理图片及Shader
    if(m_pAssetManager != NULL){
        //加载纹理图片
        loadTextureResource(m_pAssetManager);
        //加载shader
        loadShaderResource(m_pAssetManager);
    }

    //设置加载顶点、纹理等信息
    setupPlaneResource();
}

void CCSampleTexture2DScale::loadTextureResource(AAssetManager *pManager) {
    //从创建纹理图片
    CCOpenGLTexture glTexture;
    m_texId = glTexture.CreateTextureFromFile(pManager,"texture/yangchaoyue.png");
}

void CCSampleTexture2DScale::loadShaderResource(AAssetManager *pManager) {
    //加载编译器顶点着色器和片段着色器
    m_pOpenGLShader->InitShaderFromFile(pManager,
                                        "shader/texture2D/texture2Dv.glsl",
                                        "shader/texture2D/texture2Df.glsl");
}

void CCSampleTexture2DScale::setupPlaneResource() {
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
    const unsigned short planeIndexs[] = {
            0, 1, 2,  0, 2, 3
    };

    //创建并绑定VAO
    m_pVAO->Create();
    m_pVAO->Bind();

    //创建、绑定、初始化VBO数据
    m_pVBO->Create();
    m_pVBO->Bind();
    m_pVBO->SetBufferData(planeVertexs, sizeof(planeVertexs));

    //创建、绑定、初始化EBO数据
    m_pEBO->Create();
    m_pEBO->Bind();
    m_pEBO->SetBufferData(planeIndexs, sizeof(planeIndexs));

    //解释当前shader中相关的顶点坐标属性、纹理顶点属性
    int offset = 0;//坐标偏移量
    m_pOpenGLShader->SetAttributeBuffer(0,GL_FLOAT,(void *) offset,3,sizeof(CCFloat5));
    m_pOpenGLShader->EnableAttributeArray(0);

    offset += 3 * sizeof(float);

    m_pOpenGLShader->SetAttributeBuffer(1,GL_FLOAT,(void *) offset,2, sizeof(CCFloat5));
    m_pOpenGLShader->EnableAttributeArray(1);

    m_pVAO->Release();
    m_pVBO->Release();
    m_pEBO->Release();
}

void CCSampleTexture2DScale::PaintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(m_pOpenGLShader == NULL){
        return;
    }

    //初始化MVP矩阵:按照当前缩放、旋转等比例进行加载
    m_MVPMatrix = glm::mat4x4(1.0);
    UpdateMVPMatrix(m_MVPMatrix,m_AngleX,m_AngleY,(float) m_Width / (float) m_Height);

    m_pOpenGLShader->Bind();
    //将MVP矩阵传递给shader
    m_pOpenGLShader->SetUniformValue("u_mat",m_MVPMatrix);

    m_pVAO->Bind();

    //默认纹理为GL_TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_texId);

    //开始绘制
    const short* indices = (const short *) 0;
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,indices);

    glBindTexture(GL_TEXTURE_2D,0);

    m_pVAO->Release();
    m_pOpenGLShader->Release();
}

void CCSampleTexture2DScale::updateTransformMatrix(float rotateX, float rotateY, float scaleX,
                                                   float scaleY) {
    m_AngleX = (int) rotateX;//绕X轴旋转角度
    m_AngleY = (int) rotateY;//绕Y轴旋转角度
    m_ScaleX = scaleX;//x方向缩放比例
    m_ScaleY = scaleY;//y方向缩放比例
}

void CCSampleTexture2DScale::ResizeGL(int width, int height) {
    m_Width = width;
    m_Height = m_Height;
    glViewport(0,0,width,height);
}

void CCSampleTexture2DScale::Release() {
    glDeleteTextures(1,&m_texId);
    CCGLSafePtrDelete(m_pCamera);//释放摄像机
    CCGLSafePtrDelete(m_pVAO);
    CCGLSafePtrDelete(m_pVBO);
    CCGLSafePtrDelete(m_pEBO);

    CCGLSafePtrDelete(m_pOpenGLShader);
}

/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void CCSampleTexture2DScale::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY,
                                             float ratio) const {


    //投影矩阵
    //glm::mat4 mProjection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    //模型矩阵：进行缩放、旋转等操作
    glm::mat4 mModel = glm::mat4x4(1.0f);
    mModel = glm::scale(mModel,glm::vec3(m_ScaleX,m_ScaleY,1.0f));
    mModel = glm::translate(mModel, glm::vec3(angleX, angleY, 0.0f));

    mvpMatrix = m_pCamera->projectionMatrix * m_pCamera->viewMatrix * mModel;

}





