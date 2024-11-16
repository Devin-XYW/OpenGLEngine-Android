//
// Created by xuwen on 2022-07-21.
//

#ifndef OPENGLENGINE_CCOPENGLTEXTURE_H
#define OPENGLENGINE_CCOPENGLTEXTURE_H

#include "CCNDKCommonDef.h"
#include "CCNDKLogDef.h"
#include "CCImage.h"

class CCOpenGLTexture {
public:

    CCOpenGLTexture();
    ~CCOpenGLTexture();

    //获取纹理Id
    GLuint GetTextureId();
    //从文件中读取图片，创建对应的纹理
    GLuint CreateTextureFromFile(AAssetManager *assetManager, const char* fileName);

    int getWidth();

    int getHeight();

private:
    //读取文件生成纹理
    GLuint generaTexture(AAssetManager *assetManager,const char* fileName);

    //根据图片信息，创建纹理
    GLuint createOpenGLTexture(CCImage* pImg);

private:
    GLuint m_texId;//当前纹理Id
    int width = 0;
    int height = 0;
};


#endif //OPENGLENGINE_CCOPENGLTEXTURE_H
