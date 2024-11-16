//
// Created by xuwen on 2022-07-23.
//

#include "CCGLSticker.h"

CCGLSticker::CCGLSticker() {
    m_pVAO = new CCOpenGLVAO();
    m_pVBO = new CCOpenGLBuffer(CCOpenGLBuffer::VertexBuffer,CCOpenGLBuffer::StaticDraw);
    m_pEBO = new CCOpenGLBuffer(CCOpenGLBuffer::IndexBuffer,CCOpenGLBuffer::StaticDraw);

    m_pOpenGLShader = new CCOpenGLShader();
}

CCGLSticker::~CCGLSticker() {
    glDeleteTextures(1,&m_texId);

    CCGLSafePtrDelete(m_pVAO);
    CCGLSafePtrDelete(m_pVBO);
    CCGLSafePtrDelete(m_pEBO);

    CCGLSafePtrDelete(m_pOpenGLShader);
}

void CCGLSticker::InitRenderResource(AAssetManager *pManager) {

    m_pAssetManager = pManager;

    if(m_pAssetManager != NULL){
        //加载纹理图片
        loadTextureResources(m_pAssetManager);
        //加载shader
        loadShaderResource(m_pAssetManager);
    }
    //设置顶点等信息
    setupPlaneResources();
}

void CCGLSticker::loadTextureResources(AAssetManager *pManager) {
    CCOpenGLTexture glTexture;
    m_texId = glTexture.CreateTextureFromFile(pManager,"item_stickerrabit.png");
}

void CCGLSticker::loadShaderResource(AAssetManager *pManager) {
    m_pOpenGLShader->InitShaderFromFile(pManager,
                                        "shader/sticker/stickerv.glsl",
                                        "shader/sticker/stickerf.glsl");
}

void CCGLSticker::setupPlaneResources() {

    if(m_pOpenGLShader == NULL){
        return;
    }

    //贴图顶点坐标和纹理坐标
    const CCFloat5 planeVertexs[] = {
            {  -1.0,-1.0, 0.0 ,  0.0, 0.0 },
            {  -1.0, 1.0, 0.0 ,  0.0, 1.0 },
            {  1.0, -1.0,  0.0 , 1.0, 0.0 },
            {   1.0, 1.0, 0.0 ,  1.0, 1.0 },
    };
    //贴图索引坐标
    const short planeIndexs[] = {
            0,1,2,1,3,2
    };

    m_pVAO->Create();
    m_pVAO->Bind();

    m_pVBO->Create();
    m_pVBO->Bind();
    m_pVBO->SetBufferData(planeVertexs, sizeof(planeVertexs));

    m_pEBO->Create();
    m_pEBO->Bind();
    m_pEBO->SetBufferData(planeIndexs, sizeof(planeIndexs));

    int offset  = 0;
    //解释shader中的position属性，根据location设置的位置找到
    m_pOpenGLShader->SetAttributeBuffer(0,GL_FLOAT,(void *) offset,
                                        3, sizeof(CCFloat5));
    //开启position属性，默认是关闭的
    m_pOpenGLShader->EnableAttributeArray(0);

    offset += 3 * sizeof(float);//数据开始位置
    //解释shader中的uv属性，根据location设置的位置找到
    m_pOpenGLShader->SetAttributeBuffer(1,GL_FLOAT,(void *) offset,
                                        2, sizeof(CCFloat5));
    //开启uv属性，默认是关闭的
    m_pOpenGLShader->EnableAttributeArray(1);

    m_pVAO->Release();
    m_pVBO->Release();
    m_pEBO->Release();
}

void CCGLSticker::Render(CCGLCamera* pCamera){

    //生成变换矩阵
    glm::mat4x4 objectMat = glm::mat4x4(1.0);
    glm::mat4x4  objectTransMat = glm::translate(glm::mat4(1.0f)
            , glm::vec3(m_Objx+0.1, m_Objy+0.1, m_Objz));
    glm::mat4x4  objectScaleMat = glm::scale(glm::mat4(1.0f)
            ,glm::vec3(0.4f*m_ObjScale, 0.4f*0.6*m_ObjScale, 1.0) );
    //投影矩阵 * 观察矩阵 * 模板矩阵
    objectMat = pCamera->projectionMatrix * pCamera->viewMatrix * objectTransMat * objectScaleMat ;

    m_pOpenGLShader->Bind();

    m_pOpenGLShader->SetUniformValue("u_mat",objectMat);

    m_pVAO->Bind();

    //绑定纹理图片
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_texId);

    const short* indices = (const short *) 0;

    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,indices);

    glBindTexture(GL_TEXTURE_2D,0);

    m_pOpenGLShader->Release();
    m_pVAO->Release();
}

void CCGLSticker::UpdateTrackingInfo(CCGLCamera *camera, glm::vec2 pos, float scale) {
    //转换贴图的顶点坐标
    //这里pos是人脸中心位置的顶点信息
    m_Objx = pos.x;
    m_Objy = pos.y;
    m_ObjScale = scale;

    GLint  viewport[4] = {0,0,640,480};

    float x = pos.x;
    float y = pos.y;

    GLfloat z = 0;

    glGetIntegerv(GL_VIEWPORT,viewport);//获取当前屏幕尺寸

    glReadBuffer(GL_FLOAT);
    glReadPixels(x,viewport[3]-y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&m_Objz);

    //获取当前贴图需要变换的位置信息
    glm::vec3 retVec = glm::unProject(glm::vec3(x,viewport[3]-y,z),
                                      camera->viewMatrix,
                                      camera->projectionMatrix,
                                      glm::vec4(0,0,viewport[2],viewport[3]));
    m_Objx = retVec.x;
    m_Objy = retVec.y;
    m_Objz = m_Objz;
}

