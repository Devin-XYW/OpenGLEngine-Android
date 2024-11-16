//
// Created by xuwen on 2022-07-24.
//

#ifndef OPENGLENGINE_CCTEXTURERENDER_H
#define OPENGLENGINE_CCTEXTURERENDER_H

#include "CCNDKCommonDef.h"
#include "CCOpenGLBuffer.h"
#include "CCOpenGLVAO.h"
#include "CCOpenGLShader.h"
#include "CCGLCamera.h"
#include "CCOpenGLTexture.h"

/**
 * FBO离屏渲染结果到一个texture上
 */
class CCTextureRender {
public:
    CCTextureRender();
    ~CCTextureRender();

    void InitRenderResources(AAssetManager *pManager);
    void Render(CCGLCamera *camera,GLuint texID);

private:
    void loadTextureResources(AAssetManager *pManager);
    void loadShaderResources(AAssetManager *pManager);
    void setupPlaneResources();

private:
    GLuint                m_texID;

    CCOpenGLVAO*          m_pVAO;
    CCOpenGLBuffer*       m_pVBO;
    CCOpenGLBuffer*       m_pEBO;

    CCOpenGLShader*       m_pOpenGLShader;

};


#endif //OPENGLENGINE_CCTEXTURERENDER_H
