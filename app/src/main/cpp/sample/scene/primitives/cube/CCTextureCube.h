//
// Created by xuwen on 2022-07-23.
//

#ifndef OPENGLENGINE_CCTEXTURECUBE_H
#define OPENGLENGINE_CCTEXTURECUBE_H

#include "CCNDKCommonDef.h"
#include "CCOpenGLVAO.h"
#include "CCOpenGLBuffer.h"
#include "CCOpenGLShader.h"
#include "CCGLCamera.h"

class CCTextureCube {
public:
    CCTextureCube();
    ~CCTextureCube();

    void InitRenderResources(AAssetManager *pManager);
    void Render(CCGLCamera *camera);

private:
    void loadTextureResource(AAssetManager *pManager);
    void loadShaderResources(AAssetManager *pManager);

    void setupCubeResource();

private:

    float m_nValue = 0.0f;
    float m_angle = 0.0f;

    GLuint m_texId[6];//立方图6张图片

    glm::vec2 m_nResolution;

    AAssetManager* m_pAssetManager;

    CCOpenGLVAO*          m_pVAO;
    CCOpenGLBuffer*       m_pVBO;
    CCOpenGLBuffer*       m_pEBO;

    CCOpenGLShader*       m_pOpenGLShader;


};


#endif //OPENGLENGINE_CCTEXTURECUBE_H
