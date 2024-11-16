//
// Created by xuwen on 2022-07-23.
//

#ifndef OPENGLENGINE_CCGLSTICKER_H
#define OPENGLENGINE_CCGLSTICKER_H

#include "CCNDKCommonDef.h"
#include "CCOpenGLVAO.h"
#include "CCOpenGLBuffer.h"
#include "CCOpenGLShader.h"
#include "CCGLCamera.h"
#include "CCOpenGLTexture.h"

class CCGLSticker {
public:
    CCGLSticker();
    ~CCGLSticker();

    void InitRenderResource(AAssetManager *pManager);
    void Render(CCGLCamera* camera);
    //更新当前贴图的位置和缩放比例
    void UpdateTrackingInfo(CCGLCamera* camera,glm::vec2 pos, float scale);

private:
    void loadTextureResources(AAssetManager *pManager);
    void loadShaderResource(AAssetManager *pManager);

    void setupPlaneResources();

private:

    GLuint m_texId;//贴图对应的纹理Id

    glm::vec2 m_nResolution;

    //贴图在3D场景中随着脸变换的位置信息
    float m_Objx = 0;
    float m_Objy = 0;
    float m_Objz = -3;
    //贴图的缩放比例
    float m_ObjScale = 1.0;

    AAssetManager* m_pAssetManager;

    CCOpenGLVAO* m_pVAO;
    CCOpenGLBuffer* m_pVBO;
    CCOpenGLBuffer* m_pEBO;

    CCOpenGLShader* m_pOpenGLShader;

};


#endif //OPENGLENGINE_CCGLSTICKER_H
