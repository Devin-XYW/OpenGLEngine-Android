//
// Created by xuwen on 2022-07-23.
//

#include "CCTextureCube.h"
#include "CCOpenGLTexture.h"

CCTextureCube::CCTextureCube() {
    m_pVAO = new CCOpenGLVAO();
    m_pVBO = new CCOpenGLBuffer(CCOpenGLBuffer::VertexBuffer,CCOpenGLBuffer::StaticDraw);
    m_pEBO = new CCOpenGLBuffer(CCOpenGLBuffer::IndexBuffer,CCOpenGLBuffer::StaticDraw);

    m_pOpenGLShader = new CCOpenGLShader();
}

CCTextureCube::~CCTextureCube() {
    glDeleteTextures(6,m_texId);

    CCGLSafePtrDelete(m_pVAO);
    CCGLSafePtrDelete(m_pVBO);
    CCGLSafePtrDelete(m_pEBO);

    CCGLSafePtrDelete(m_pOpenGLShader);
}

void CCTextureCube::InitRenderResources(AAssetManager *pManager) {
    m_pAssetManager = pManager;

    if(m_pAssetManager != NULL){
        //加载纹理图片
        loadTextureResource(m_pAssetManager);
        //加载shader资源
        loadShaderResources(m_pAssetManager);
    }

    setupCubeResource();
}

void CCTextureCube::loadTextureResource(AAssetManager *pManager) {

    for(int i=0;i<6;i++){
        char nameBuffer[6];
        memset(nameBuffer,0, sizeof(nameBuffer));
        sprintf(nameBuffer,"%d.png",i+1);
        nameBuffer[5] = '\0';
        LOGI("Image Name:%s",nameBuffer);
        CCOpenGLTexture glTexture;
        //加载纹理
        m_texId[i] = glTexture.CreateTextureFromFile(pManager,nameBuffer);
    }
}

void CCTextureCube::loadShaderResources(AAssetManager *pManager) {
    m_pOpenGLShader->InitShaderFromFile(pManager
                                        ,"shader/cube/cubev.glsl"
                                        ,"shader/cube/cubef.glsl");
}

void CCTextureCube::setupCubeResource() {

    if(m_pOpenGLShader == NULL){
        return;
    }

    //立方体点坐标和纹理坐标
    const CCFloat5 cubeVertexs[]  = {
            {  -1.0,-1.0, 1.0 ,  0.0, 0.0 },
            {  -1.0, 1.0, 1.0 ,  0.0, 1.0 },
            {  1.0, -1.0,  1.0 , 1.0, 0.0 },
            {   1.0, 1.0, 1.0 ,  1.0, 1.0 },

            {   1.0,-1.0, -1.0,   0,  0  },
            {   1.0, 1.0, -1.0,   0,  1  },
            {   -1.0,-1.0, -1.0,   1,  0 },
            {   -1.0, 1.0, -1.0,   1,  1 },


            {   -1.0, -1.0, -1.0,  0,  0 },
            {   -1.0, 1.0, -1.0,   0,  1 },
            {   -1.0, -1.0,  1.0,  1,  0 },
            {   -1.0, 1.0, 1.0,    1,  1 },

            {   1.0,-1.0,  1.0,    0,  0 },
            {   1.0, 1.0,  1.0,    0,  1 },
            {   1.0, -1.0,  -1.0,  1,  0 },
            {   1.0, 1.0, -1.0,    1,  1 },

            {   -1.0, 1.0,  1.0,   0,  0 },
            {   -1.0, 1.0,  -1.0,  0,  1 },
            {   1.0, 1.0, 1.0,     1,  0 },
            {   1.0, 1.0, -1.0,    1,  1 },

            {   -1.0, -1.0, -1.0,  0,  0 },
            {   -1.0, -1.0, 1.0,   0,  1 },
            {   1.0, -1.0, -1.0,   1,  0 },
            {   1.0, -1.0, 1.0,    1,  1 }
    };

    //立方体索引坐标
    const short cubeIndexs[]= {
            0, 1, 2,  2, 1, 3,
            4, 5, 6,  6, 5, 7,
            8, 9, 10, 10, 9,11,
            12,13,14, 14,13,15,
            16,17,18, 18,17,19,
            20,21,22, 22,21,23,
    };

    m_pVAO->Create();
    m_pVAO->Bind();

    m_pVBO->Create();
    m_pVBO->Bind();
    m_pVBO->SetBufferData(cubeVertexs, sizeof(cubeVertexs));

    m_pEBO->Create();
    m_pEBO->Bind();
    m_pEBO->SetBufferData(cubeIndexs, sizeof(cubeIndexs));

    int offset = 0;

    //设置shader中传入顶点坐标
    m_pOpenGLShader->SetAttributeBuffer(0,GL_FLOAT,(void *) offset
                                        ,3, sizeof(CCFloat5));
    m_pOpenGLShader->EnableAttributeArray(0);

    offset += 3 * sizeof(float);

    //设置shader中传递纹理坐标
    m_pOpenGLShader->SetAttributeBuffer(1,GL_FLOAT,(void *) offset
                                        ,2, sizeof(CCFloat5));
    m_pOpenGLShader->EnableAttributeArray(1);

    m_pVAO->Release();
    m_pVBO->Release();
    m_pEBO->Release();
}

void CCTextureCube::Render(CCGLCamera *camera) {

   glCullFace(GL_BACK);//不绘制隐藏的面
   glFrontFace(GL_CW);

   m_angle += 0.05f;

   //变换矩阵 = 投影矩阵 * 观察矩阵 * 模板矩阵
   glm::mat4x4  objectMat = glm::mat4x4(1.0);
   glm::mat4x4  objectTransMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4));
   glm::mat4x4  objectRotMat = glm::rotate(glm::mat4(1.0f),m_angle,glm::vec3(1.0f, 1.0f, 1.0) );
   glm::mat4x4  objectScaleMat = glm::scale(glm::mat4(1.0f),glm::vec3(0.3f, 0.2f, 0.3) );

   objectMat = camera->projectionMatrix * camera->viewMatrix * objectTransMat * objectScaleMat* objectRotMat ;

   m_pOpenGLShader->Bind();

   m_pOpenGLShader->SetUniformValue("u_mat",objectMat);

   m_pVAO->Bind();

   for(int i=0;i<6;i++){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,m_texId[i]);

        const short* indices = (const short *) (i*6* sizeof(short));
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,indices);
        glBindTexture(GL_TEXTURE_2D,0);
   }

   m_pOpenGLShader->Release();
   m_pVAO->Release();
}







