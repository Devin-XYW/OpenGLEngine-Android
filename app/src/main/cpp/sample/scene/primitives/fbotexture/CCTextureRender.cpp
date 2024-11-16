//
// Created by xuwen on 2022-07-24.
//

#include "CCTextureRender.h"

CCTextureRender::CCTextureRender() {
    m_pVAO = new CCOpenGLVAO();
    m_pVBO = new CCOpenGLBuffer(CCOpenGLBuffer::VertexBuffer,CCOpenGLBuffer::StaticDraw);
    m_pEBO = new CCOpenGLBuffer(CCOpenGLBuffer::IndexBuffer,CCOpenGLBuffer::StaticDraw);

    m_pOpenGLShader = new CCOpenGLShader();
}

CCTextureRender::~CCTextureRender() {
    CCGLSafePtrDelete(m_pVAO);
    CCGLSafePtrDelete(m_pVBO);
    CCGLSafePtrDelete(m_pEBO);

    CCGLSafePtrDelete(m_pOpenGLShader);
}

void CCTextureRender::InitRenderResources(AAssetManager *pManager) {
    if(pManager == NULL){
        return;
    }

    loadTextureResources(pManager);
    loadShaderResources(pManager);

    setupPlaneResources();
}

void CCTextureRender::loadShaderResources(AAssetManager *pManager)
{
    m_pOpenGLShader->InitShaderFromFile(pManager,"shader/fbo/sampletexturev.glsl","shader/fbo/sampletexturef.glsl");
}

void CCTextureRender::loadTextureResources(AAssetManager *pManager)
{
//    CCOpenGLTexture glTexture;
//    m_texID = glTexture.CreateTextureFromFile(pManager,"item_stickerrabit.png");
}

void CCTextureRender::setupPlaneResources() {
    if(m_pOpenGLShader == NULL){
        return;
    }

    //一个简单的平面
    const CCFloat5 planeVertexs[]  = {
            {  -1.0,-1.0, 0.0 ,  0.0, 0.0 },
            {  -1.0, 1.0, 0.0 ,  0.0, 1.0 },
            {  1.0, -1.0,  0.0 , 1.0, 0.0 },
            {   1.0, 1.0, 0.0 ,  1.0, 1.0 },
    };

    const unsigned short planeIndexs[]= {
            0, 1, 2,  1, 3, 2
    };

    m_pVAO->Create();
    m_pVAO->Bind();

    m_pVBO->Create();
    m_pVBO->Bind();
    m_pVBO->SetBufferData(planeVertexs,sizeof(planeVertexs));


    m_pEBO->Create();
    m_pEBO->Bind();
    m_pEBO->SetBufferData(planeIndexs,sizeof(planeIndexs));

    int offset = 0;

    m_pOpenGLShader->SetAttributeBuffer(0,GL_FLOAT, (void *)offset, 3, sizeof(CCFloat5));
    m_pOpenGLShader->EnableAttributeArray(0);

    offset += 3 * sizeof(float);

    m_pOpenGLShader->SetAttributeBuffer(1,GL_FLOAT, (void *)offset, 2, sizeof(CCFloat5));
    m_pOpenGLShader->EnableAttributeArray(1);

    m_pVAO->Release();
    m_pVBO->Release();
    m_pEBO->Release();
}

void CCTextureRender::Render(CCGLCamera* pCamera,GLuint textureID)
{
    if(m_pOpenGLShader == NULL){
        return;
    }


    glm::mat4x4  objectMat = glm::mat4x4(1.0);
    glm::mat4x4  objectTransMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0));
    objectMat = objectMat * objectTransMat;

    objectMat = pCamera->projectionMatrix * pCamera->viewMatrix * objectMat ;


    m_pOpenGLShader->Bind();

    m_pOpenGLShader->SetUniformValue("u_mat",objectMat);

    m_pVAO->Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,textureID);

    const short* indices =(const short *) 0;
    glDrawElements(GL_TRIANGLES, 6,  GL_UNSIGNED_SHORT, indices);

    glBindTexture(GL_TEXTURE_2D,0);

    m_pOpenGLShader->Release();
    m_pVAO->Release();
}





