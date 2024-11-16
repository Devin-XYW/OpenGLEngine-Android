//
// Created by xuwen on 2022-11-27.
//

#ifndef OPENGLENGINE_MODEL3DSAMPLE_H
#define OPENGLENGINE_MODEL3DSAMPLE_H


#include "CCGLCamera.h"
#include "CCGLVideoRender.h"
#include "CCTextureCube.h"
#include "CCGLSticker.h"
#include "CCVideoWriter.h"
#include "GLBaseSample.h"
#include <shader.h>
#include <model.h>
#define MATH_PI 3.1415926535897932384626433832802
#define DEFAULT_OGL_ASSETS_DIR "/data/data/com.xuwen.openglengine/cache"

class Model3DSample: public GLBaseSample {

public:
    Model3DSample();
    ~Model3DSample();

    //初始化当前AssetsManger
    void SetupAssetManager(AAssetManager *assetManager,std::string path);

    void InitGL();//SurfaceView创建时进行初始化
    void PaintGL();//渲染相关图像
    void ResizeGL(int width,int height);//SurfaceView画面改变时调用
    void Release();

    void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);
    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    void initSceneResources();//初始化资源

private:
    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;
    Shader *m_pShader;
    Model *m_pModel;

    AAssetManager* m_pAssetManager;
    string asset_dir = "";

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

    int mScreenW;
    int mScreenH;
};


#endif //OPENGLENGINE_MODEL3DSAMPLE_H
