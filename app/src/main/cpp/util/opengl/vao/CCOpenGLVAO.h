//
// Created by xuwen on 2022-07-21.
//

#ifndef OPENGLENGINE_CCOPENGLVAO_H
#define OPENGLENGINE_CCOPENGLVAO_H

#include "CCNDKCommonDef.h"

class CCOpenGLVAO {
public:

    CCOpenGLVAO();
    ~CCOpenGLVAO();

    void Bind();
    void Release();
    void Create();

private:
    GLuint m_vaoId;
};


#endif //OPENGLENGINE_CCOPENGLVAO_H
