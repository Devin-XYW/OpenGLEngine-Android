//
// Created by xuwen on 2022-09-05.
//

#ifndef OPENGLENGINE_CCGREENSCREENTEXTURE_H
#define OPENGLENGINE_CCGREENSCREENTEXTURE_H

#include "GLBaseSample.h"
class CCGreenScreenTexture : public GLBaseSample{
public:
    CCGreenScreenTexture();
    ~CCGreenScreenTexture();

    //初始化当前AssetManager
    void SetupAssetManager(AAssetManager *assetManager,std::string path);

    void InitGL(); //SurfaceView创建时进行初始化
    void PaintGL();//渲染相关图像
    void ResizeGL(int width,int height);
    void Release();

private:
    void loadTextureResource(AAssetManager *pManager);//加载纹理图片
    void loadShaderResource(AAssetManager *pManager);//加载shader资源
    void setupPlaneResource();//加载顶点、纹理坐标等资源

private:
    std::string m_directoryPath;//当前asset文件目录路径
    AAssetManager *m_pAssetManager;

    GLuint m_texId_1;//转场纹理贴图1
    GLuint m_texId_2;//转场纹理贴图2

    CCGLCamera* m_pCamera;//3D场景中摄像机观察者

    CCOpenGLVAO* m_pVAO;//声明VAO
    CCOpenGLBuffer* m_pVBO;//声明VBO
    CCOpenGLBuffer* m_pEBO;//声明EBO

    float m_offset = 0.0f;//渐变值
    int mVideoWidth = 0;//图片宽
    int mVideoHeight = 0;//图片高

    CCOpenGLShader* m_pOpenGLShader;//创建当前shader

};


#endif //OPENGLENGINE_CCGREENSCREENTEXTURE_H
