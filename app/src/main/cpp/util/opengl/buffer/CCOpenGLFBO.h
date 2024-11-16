//
// Created by xuwen on 2022-07-21.
//

#ifndef OPENGLENGINE_CCOPENGLFBO_H
#define OPENGLENGINE_CCOPENGLFBO_H

#include "CCNDKCommonDef.h"

class CCOpenGLFBO {

public:
    CCOpenGLFBO();
    ~CCOpenGLFBO();

    void Bind();
    void Release();

    GLuint GetTextureId();

    void CreateWithSize(int width,int height);

private:
    GLuint m_fboId;//当前FBO 对应的id
    GLuint m_depthBufferId;
    GLuint m_textureId;
};


#endif //OPENGLENGINE_CCOPENGLFBO_H
