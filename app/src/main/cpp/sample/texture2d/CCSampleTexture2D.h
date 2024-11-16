//
// Created by xuwen on 2022-08-07.
//

#ifndef OPENGLENGINE_CCSAMPLETEXTURE2D_H
#define OPENGLENGINE_CCSAMPLETEXTURE2D_H

#include "GLBaseSample.h"

class CCSampleTexture2D: public GLBaseSample{
public:
    CCSampleTexture2D();
    ~CCSampleTexture2D();

    //初始化当前AssetsManger
    void SetupAssetManager(AAssetManager *assetManager,std::string path);

    void InitGL();//SurfaceView创建时进行初始化
    void PaintGL();//渲染相关图像
    void ResizeGL(int width,int height);//SurfaceView画面改变时调用
    void Release();

private:
    void loadTextureResource(AAssetManager *pManager);//加载纹理图片资源
    void loadShaderResource(AAssetManager *pManager);//加载Shader资源
    void setupPlaneResource();//加载顶点、纹理坐标等资源

private:
    std::string m_directoryPath;//当前asset文件目录路径
    AAssetManager* m_pAssetManager;

    GLuint m_texId;//纹理贴图对应的纹理Id
    CCGLCamera* m_pCamera;//3D场景中的摄像机观察者

    CCOpenGLVAO* m_pVAO;//声明VAO
    CCOpenGLBuffer* m_pVBO;//声明VBO
    CCOpenGLBuffer* m_pEBO;//声明EBO

    CCOpenGLShader* m_pOpenGLShader;//创建当前shader
};


#endif //OPENGLENGINE_CCSAMPLETEXTURE2D_H
