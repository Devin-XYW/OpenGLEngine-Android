//
// Created by xuwen on 2022-08-07.
//

#ifndef OPENGLENGINE_CCSAMPLETEXTURE2DSCALE_H
#define OPENGLENGINE_CCSAMPLETEXTURE2DSCALE_H

#include "GLBaseSample.h"

class CCSampleTexture2DScale : public GLBaseSample{
public:
    CCSampleTexture2DScale();
    ~CCSampleTexture2DScale();

    //初始化当前AssetsManger
    void SetupAssetManager(AAssetManager *assetManager,std::string path);

    void InitGL();//SurfaceView创建时进行初始化
    void PaintGL();//渲染相关图像
    void ResizeGL(int width,int height);//SurfaceView画面改变时调用

    //更新缩放、旋转等相关数据
    void updateTransformMatrix(float rotateX,float rotateY,float scaleX,float scaleY);

    void Release();

private:
    void loadTextureResource(AAssetManager *pManager);//加载纹理图片资源
    void loadShaderResource(AAssetManager *pManager);//加载Shader资源
    void setupPlaneResource();//加载顶点、纹理坐标等资源

    //根据当前手指进行的缩放比例进行调整MVP矩阵
    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;
private:

    AAssetManager* m_pAssetManager;//当前asset目录

    GLuint m_texId;//纹理贴图对应的纹理Id
    CCGLCamera* m_pCamera;//3D场景中的摄像机观察者
    glm::mat4 m_MVPMatrix = glm::mat4x4(1.0);

    CCOpenGLVAO* m_pVAO;//声明VAO
    CCOpenGLBuffer* m_pVBO;//声明VBO
    CCOpenGLBuffer* m_pEBO;//声明EBO

    CCOpenGLShader* m_pOpenGLShader;//创建当前shader

    int m_AngleX;//x方向旋转角度
    int m_AngleY;//y方向旋转角度
    int m_ScaleX;//x方向缩放大小
    int m_ScaleY;//y方向缩放大小

    int m_Width;//屏幕宽度
    int m_Height;//屏幕高度
};


#endif //OPENGLENGINE_CCSAMPLETEXTURE2DSCALE_H
