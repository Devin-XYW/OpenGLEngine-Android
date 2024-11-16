//
// Created by xuwen on 2022-12-03.
//

#ifndef OPENGLENGINE_MODEL3DANIMSAMPLE_H
#define OPENGLENGINE_MODEL3DANIMSAMPLE_H

#include "CCGLCamera.h"
#include "CCGLVideoRender.h"
#include "CCTextureCube.h"
#include "CCGLSticker.h"
#include "CCVideoWriter.h"
#include "GLBaseSample.h"
#include <shader.h>
#include <model.h>
#include "animator.h"
#include <model_animation.h>
#include "glm/detail/type_mat4x4.hpp"
#define MATH_PI 3.1415926535897932384626433832802
#define DEFAULT_OGL_ASSETS_DIR "/data/data/com.xuwen.openglengine/cache"

class Model3DAnimSample : public GLBaseSample{

public:
    Model3DAnimSample();
    ~Model3DAnimSample();

    //初始化当前AssetsManager
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
    ModelAnim *m_pModel;
    Animation *m_pAnimation;
    Animator *m_pAnimator;

    AAssetManager* m_pAssetManager;
    string asset_dir = "";

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

    int mScreenW;
    int mScreenH;
};


#endif //OPENGLENGINE_MODEL3DANIMSAMPLE_H
