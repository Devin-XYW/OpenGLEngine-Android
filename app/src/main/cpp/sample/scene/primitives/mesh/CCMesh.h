#ifndef CCOPENCV_CCMESH_H
#define CCOPENCV_CCMESH_H


#include "CCNDKCommonDef.h"
#include "CCOpenGLVAO.h"
#include "CCOpenGLBuffer.h"
#include "CCOpenGLShader.h"
#include "CCOpenGLTexture.h"
#include "CCGLCamera.h"


typedef struct VertexData
{
    VertexData ()
    {
    }
    VertexData (glm::vec3 p, glm::vec2 t, glm::vec3 n) :
            position(p), textCoord(t), normal(n)
    {
    }

    glm::vec3 position;//顶点坐标
    glm::vec2 textCoord;//纹理坐标
    glm::vec3 normal;//法向量
    glm::vec3 tangent;//切线
    glm::vec3 bitangent;//双切线

}CCVertexData;


class CCMesh {
public:
    CCMesh();
    ~CCMesh();

    void InitRenderResources(AAssetManager *pManager,const std::vector<CCVertexData> &vertData, const std::vector<GLuint> &indexes);
    void Render(CCGLCamera* camera);

    void UpdateTrackingInfo(CCGLCamera* pCamera,glm::vec2 pos,float scale);

private:
    void loadTextureResources(AAssetManager *pManager);
    void loadShaderResources(AAssetManager *pManager);


private:


    float       m_Objx = 0;
    float       m_Objy = 0;
    float       m_Objz =-1;
    float       m_ObjScale = 1.0;

    int         m_indexBuffSize = 0;

    GLuint           m_diffuseId;
    GLuint           m_normalId;
    GLuint           m_specularId;


    AAssetManager*        m_pAssetManager;

    CCOpenGLVAO*          m_pVAO;
    CCOpenGLBuffer*       m_pVBO;
    CCOpenGLBuffer*       m_pEBO;

    CCOpenGLShader*       m_pOpenGLShader;

};


#endif //CCOPENCV_CCMESH_H
