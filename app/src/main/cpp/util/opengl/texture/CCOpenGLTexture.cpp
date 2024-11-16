//
// Created by xuwen on 2022-07-21.
//

#include "CCOpenGLTexture.h"

CCOpenGLTexture::CCOpenGLTexture() {

}

CCOpenGLTexture::~CCOpenGLTexture() {

}

GLuint CCOpenGLTexture::GetTextureId() {
    return m_texId;
}

GLuint CCOpenGLTexture::CreateTextureFromFile(AAssetManager *assetManager, const char *fileName) {

    //根据assets和图片文件名生成纹理
    m_texId = generaTexture(assetManager,fileName);
    return m_texId;
}

GLuint CCOpenGLTexture::generaTexture(AAssetManager *assetManager, const char *fileName) {

    //打开Asset文件夹下的文件
    AAsset *pathAsset = AAssetManager_open(assetManager,fileName,AASSET_MODE_UNKNOWN);
    if (NULL == pathAsset){
        LOGF("asset is NULL");
        return -1;
    }
    //得到文件长度
    off_t bufferSize = AAsset_getLength(pathAsset);
    LOGD("buffer size is %ld", bufferSize);

    // 得到文件对应的 Buffer
    unsigned char *imgBuff = (unsigned char *) AAsset_getBuffer(pathAsset);

    //创建CCImage对象
    CCImage* glImage = new CCImage();
    //将buffer中的数据读取到CCImage中
    glImage->ReadFromBuffer(imgBuff,bufferSize);
    GLuint texId = createOpenGLTexture(glImage);

    width = glImage->GetWidth();
    height = glImage->GetHeight();

    //删除图片
    delete glImage;

//    //释放buffer
//    if (imgBuff){
//        free(imgBuff);
//        imgBuff = NULL;
//    }

    //关闭assets
    AAsset_close(pathAsset);

    return texId;
}

GLuint CCOpenGLTexture::createOpenGLTexture(CCImage *pImg) {
    if(pImg == NULL){
        return -1;
    }

    GLuint textureId;
    //开启纹理
    glEnable(GL_TEXTURE_2D);
    //生成纹理索引
    glGenTextures(1,&textureId);
    //绑定纹理索引，之后的操作都针对当前纹理索引
    glBindTexture(GL_TEXTURE_2D,textureId);

    //指当纹理图象被使用到一个大于它的形状上时
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    //指当纹理图象被使用到一个小于或等于它的形状上时
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //指定参数，生成纹理
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA
                 ,pImg->GetWidth(),pImg->GetHeight()
                 ,0,GL_RGBA,GL_UNSIGNED_BYTE
                 ,pImg->GetData());

    return textureId;
}

int CCOpenGLTexture::getWidth() {
    return width;
}

int CCOpenGLTexture::getHeight() {
    return height;
}

