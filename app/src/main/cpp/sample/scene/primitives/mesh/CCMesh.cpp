#include "CCMesh.h"

CCMesh::CCMesh()
{
    m_pVAO = new CCOpenGLVAO();
    m_pVBO = new CCOpenGLBuffer(CCOpenGLBuffer::VertexBuffer,CCOpenGLBuffer::StaticDraw);
    m_pEBO = new CCOpenGLBuffer(CCOpenGLBuffer::IndexBuffer,CCOpenGLBuffer::StaticDraw);

    m_pOpenGLShader = new CCOpenGLShader();
}

CCMesh::~CCMesh()
{
    glDeleteTextures(1,&m_diffuseId);
    glDeleteTextures(1,&m_normalId);
    glDeleteTextures(1,&m_specularId);

    CCGLSafePtrDelete(m_pVAO);
    CCGLSafePtrDelete(m_pVBO);
    CCGLSafePtrDelete(m_pEBO);

    CCGLSafePtrDelete(m_pOpenGLShader);
}

void CCMesh::InitRenderResources(AAssetManager *pManager,const std::vector<CCVertexData> &vertData, const std::vector<GLuint> &indexes)
{
    if(pManager == NULL){
        return;
    }

    loadTextureResources(pManager);
    loadShaderResources(pManager);

    m_indexBuffSize = indexes.size() ;


    m_pVAO->Create();
    m_pVAO->Bind();

    m_pVBO->Create();
    m_pVBO->Bind();
    m_pVBO->SetBufferData(vertData.data(),vertData.size() * sizeof (CCVertexData));


    m_pEBO->Create();
    m_pEBO->Bind();
    m_pEBO->SetBufferData(indexes.data(), indexes.size() * sizeof (GLuint));

    int offset = 0;

    m_pOpenGLShader->SetAttributeBuffer(0, GL_FLOAT, (void *)offset, 3, sizeof(CCVertexData));
    m_pOpenGLShader->EnableAttributeArray(0);

    offset += sizeof (glm::vec3);

    m_pOpenGLShader->SetAttributeBuffer(1, GL_FLOAT, (void *)offset, 2, sizeof(CCVertexData));
    m_pOpenGLShader->EnableAttributeArray(1);

    offset += sizeof (glm::vec2);

    m_pOpenGLShader->SetAttributeBuffer(2, GL_FLOAT, (void *)offset, 3, sizeof (CCVertexData));
    m_pOpenGLShader->EnableAttributeArray(2);

    offset += sizeof (glm::vec3);

    m_pOpenGLShader->SetAttributeBuffer(3, GL_FLOAT, (void *)offset, 3, sizeof (CCVertexData));
    m_pOpenGLShader->EnableAttributeArray(3);

    offset += sizeof (glm::vec3);

    m_pOpenGLShader->SetAttributeBuffer(4, GL_FLOAT, (void *)offset, 3, sizeof (CCVertexData));
    m_pOpenGLShader->EnableAttributeArray(4);

    m_pVAO->Release();
    m_pVBO->Release();
    m_pEBO->Release();
}


void CCMesh::Render(CCGLCamera* pCamera)
{


    glm::mat4x4  modelMatrix = glm::mat4x4(1.0);

    glm::mat4x4  objectTransMat = glm::translate(glm::mat4(1.0f), glm::vec3(m_Objx+0.1, m_Objy-0.2, m_Objz));
    glm::mat4x4  objectScaleMat = glm::scale(glm::mat4(1.0f),glm::vec3(0.25f*m_ObjScale, 0.25f*0.6*m_ObjScale, 0.25f*m_ObjScale) );

    modelMatrix = objectTransMat * objectScaleMat ;


    m_pOpenGLShader->Bind();

    //模型矩阵
    m_pOpenGLShader->SetUniformValue("u_modelMatrix", modelMatrix);
    //观察矩阵
    m_pOpenGLShader->SetUniformValue("u_viewMatrix", pCamera->viewMatrix);
    //投影矩阵
    m_pOpenGLShader->SetUniformValue("u_projectionMatrix", pCamera->projectionMatrix);


    glActiveTexture(GL_TEXTURE0);
    //模型颜色贴图
    m_pOpenGLShader->SetUniformValue("texture_diffuse", 0);
    glBindTexture(GL_TEXTURE_2D,m_diffuseId);


    glActiveTexture(GL_TEXTURE1);
    //模型法线贴图
    m_pOpenGLShader->SetUniformValue("texture_normal", 1);
    glBindTexture(GL_TEXTURE_2D,m_normalId);


    glActiveTexture(GL_TEXTURE2);
    //模型高光贴图
    m_pOpenGLShader->SetUniformValue("texture_specular", 2);
    glBindTexture(GL_TEXTURE_2D,m_specularId);

    //反射光强度因子
    m_pOpenGLShader->SetUniformValue("m_shiness", 32.0f);
    //观察者位置
    m_pOpenGLShader->SetUniformValue("u_viewPos", pCamera->GetEyePosition());

    //环境光照颜色
    m_pOpenGLShader->SetUniformValue("myLight.m_ambient", glm::vec3(0.5,0.5,0.5));
    //漫反射光照颜色
    m_pOpenGLShader->SetUniformValue("myLight.m_diffuse", glm::vec3(0.8,0.8,0.8));
    //反射光光照颜色
    m_pOpenGLShader->SetUniformValue("myLight.m_specular", glm::vec3(0.9,0.9,0.9));

    //光照位置
    m_pOpenGLShader->SetUniformValue("myLight.m_pos", glm::vec3(5.0,5.0,5.0));
    m_pOpenGLShader->SetUniformValue("myLight.m_c", 1.0f);
    m_pOpenGLShader->SetUniformValue("myLight.m_l", 0.09f);
    m_pOpenGLShader->SetUniformValue("myLight.m_q", 0.032f);


    m_pVAO->Bind();



    const short* indices =(const short *) 0;
    glDrawElements(GL_TRIANGLES, m_indexBuffSize,  GL_UNSIGNED_INT, indices);

    glBindTexture(GL_TEXTURE_2D,0);

    m_pOpenGLShader->Release();
    m_pVAO->Release();

}

void  CCMesh::UpdateTrackingInfo(CCGLCamera* pCamera,glm::vec2 pos,float scale)
{

    m_Objx = pos.x;
    m_Objy = pos.y;

    m_ObjScale = scale;

    GLint viewport[4] = {0,0,640,480};

    float x = pos.x;
    float y = pos.y;

    GLfloat  z = 0;

    glGetIntegerv( GL_VIEWPORT, viewport );

    glReadBuffer(GL_FRONT);
    glReadPixels( x, viewport[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &m_Objz );


    glm::vec3 retMat = glm::unProject(glm::vec3(x, viewport[3]-y, z), pCamera->viewMatrix, pCamera->projectionMatrix,
                                      glm::vec4(0,0,viewport[2],viewport[3]));
    m_Objx = retMat.x;
    m_Objy = retMat.y;
    m_Objz = m_Objz;

}

void CCMesh::loadTextureResources(AAssetManager *pManager)
{
    CCOpenGLTexture glTexture;
    //颜色贴图: 绘制3D模型的颜色
    m_diffuseId = glTexture.CreateTextureFromFile(pManager,"texture/monkey_diffuse.png");
    //法线贴图：绘制3D模型表面一些细节，每个像素存储的是该片元的法向量
    //通过模拟物体表面的阴暗使得渲染出来的物体更有凹凸感，而且性能开销很小
    //垂直于物体表面的法向量影响表面的凹凸，表面只有一种法线方向就没有凹凸感
    //将法向量的x,y,z元素存储到纹理中，代替颜色的rgb元素值
    m_normalId = glTexture.CreateTextureFromFile(pManager,"texture/monkey_normal.png");
    //高光贴图：控制3D模型在那些位置可以存着一些高光
    m_specularId = glTexture.CreateTextureFromFile(pManager,"texture/monkey_specular.png");
//    m_diffuseId = glTexture.CreateTextureFromFile(pManager,"tortoise_diffuse.png");
//    m_normalId = glTexture.CreateTextureFromFile(pManager,"tortoise_normal.png");
//    m_specularId = glTexture.CreateTextureFromFile(pManager,"tortoise_specular.png");
//    m_diffuseId = glTexture.CreateTextureFromFile(pManager,"dragon_diffuse.png");
//    m_normalId = glTexture.CreateTextureFromFile(pManager,"dragon_normal.png");
//    m_specularId = glTexture.CreateTextureFromFile(pManager,"dragon_specular.png");

}
void CCMesh::loadShaderResources(AAssetManager *pManager)
{
    m_pOpenGLShader->InitShaderFromFile(pManager,"shader/mesh/objloaderv.glsl","shader/mesh/objloaderf.glsl");
}