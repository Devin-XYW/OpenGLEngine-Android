//
// Created by xuwen on 2022-12-03.
//

#include "Model3DAnimSample.h"

Model3DAnimSample::Model3DAnimSample() {
    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_pModel = nullptr;
    m_pShader = nullptr;
}

Model3DAnimSample::~Model3DAnimSample(){

}

void Model3DAnimSample::SetupAssetManager(AAssetManager *assetManager, std::string path) {
    m_pAssetManager = assetManager;
    asset_dir = path;
}

void Model3DAnimSample::InitGL() {
    if(m_pModel != nullptr && m_pShader != nullptr)
        return;
    initSceneResources();
}

void Model3DAnimSample::initSceneResources() {

    //加载含有动画的3D模型
    std::string path(DEFAULT_OGL_ASSETS_DIR);
    std::string modelPath(path + "/model/vampire/dancing_vampire.dae");
    m_pModel = new ModelAnim(modelPath);
    //读取动画
    m_pAnimation = new Animation(modelPath, m_pModel);
    //生成动画管理类
    m_pAnimator = new Animator(m_pAnimation);

    //加载当前Shader
    m_pShader = new Shader(m_pAssetManager,"shader/3dmodel/3dmodelshaderv.glsl","shader/3dmodel/3dmodelshaderf.glsl");
}

void Model3DAnimSample::PaintGL() {

    if(m_pModel == nullptr || m_pShader == nullptr) return;

    //首先更新动画
    //这个值为0.03原因：首先基于1s30帧，所以则算下来，一帧0.03s
    float deltaTime = 0.03f;
    //根据时间戳，计算动画矩阵
    //m_pAnimator->UpdateAnimation(deltaTime);


    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    //1. 根据用户手势，得到MVP矩阵
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float)mScreenW / mScreenH);

    //2. 把各种参数传给着色器shader
    m_pShader->use();
    m_pShader->setMat4("u_MVPMatrix", m_MVPMatrix);
    m_pShader->setMat4("u_ModelMatrix", m_ModelMatrix);
    m_pShader->setVec3("lightPos", glm::vec3(0, 0, m_pModel->GetMaxViewDistance()));
    m_pShader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    m_pShader->setVec3("viewPos", glm::vec3(0, 0, m_pModel->GetMaxViewDistance()));

    //重点，获得动画矩阵
    auto transforms = m_pAnimator->GetFinalBoneMatrices();

    string transformStr = "";

    //传递给vertex shader, 用于计算动画之后的新顶点坐标
    for (int i = 0; i < transforms.size(); ++i){
        m_pShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        //将transforms[i]转换为字符串，方便打印
        transformStr += glm::to_string(transforms[i]) + ",";
    }

    LOGCATI("xuwen finalBonesMatrices = %s", transformStr.c_str());

    //调用DrawCall，逐网格绘制
    m_pModel->Draw((*m_pShader));
    LOGCATE("Draw done");

}

void Model3DAnimSample::ResizeGL(int width, int height) {
    mScreenW = width;
    mScreenH = height;
}

void Model3DAnimSample::Release() {
    LOGCATE("Model3DSample::Destroy");
    if (m_pModel != nullptr) {
        m_pModel->Destroy();
        delete m_pModel;
        m_pModel = nullptr;
    }

    if (m_pShader != nullptr) {
        m_pShader->Destroy();
        delete m_pShader;
        m_pShader = nullptr;
    }
}

void Model3DAnimSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGCATE("Model3DAnimSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, m_pModel->GetMaxViewDistance() * 4);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, m_pModel->GetMaxViewDistance() * 1.8f), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, -m_pModel->GetAdjustModelPosVec());
    m_ModelMatrix = Model;
    mvpMatrix = Projection * View * Model;

}

void Model3DAnimSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX,
                                              float scaleY) {
    //记录当前滑动距离
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    //记录当前缩放值
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}


