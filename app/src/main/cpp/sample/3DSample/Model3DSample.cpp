//
// Created by xuwen on 2022-11-27.
//

#include "Model3DSample.h"

Model3DSample::Model3DSample() {
    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_pModel = nullptr;
    m_pShader = nullptr;
}

Model3DSample::~Model3DSample() {
}

void Model3DSample::SetupAssetManager(AAssetManager *assetManager, std::string path) {
    m_pAssetManager = assetManager;
    asset_dir = path;
}

void Model3DSample::InitGL() {
    if(m_pModel != nullptr && m_pShader != nullptr)
        return;
    initSceneResources();
}

void Model3DSample::PaintGL() {
    if(m_pModel == nullptr || m_pShader == nullptr) return;
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    //更新MVP矩阵
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float)mScreenW / mScreenH);

    //开始进行绘制
    m_pShader->use();
    m_pShader->setMat4("u_MVPMatrix", m_MVPMatrix);
    m_pShader->setMat4("u_ModelMatrix", m_ModelMatrix);
    m_pShader->setVec3("lightPos", glm::vec3(0, 0, m_pModel->GetMaxViewDistance()));
    m_pShader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    m_pShader->setVec3("viewPos", glm::vec3(0, 0, m_pModel->GetMaxViewDistance()));
    m_pModel->Draw((*m_pShader));

    LOGCATE("Model3DSample::Draw() done");
}

void Model3DSample::ResizeGL(int width, int height) {
    mScreenW = width;
    mScreenH = height;
}

void Model3DSample::Release() {
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

void Model3DSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX,
                                          float scaleY) {
    //记录当前滑动距离
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    //记录当前缩放值
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void Model3DSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGCATE("Model3DSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //将滑动距离转换为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    //计算投影矩阵：这里使用模型中定义的
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, m_pModel->GetMaxViewDistance() * 4);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    //计算视图矩阵：摄像机位置
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, m_pModel->GetMaxViewDistance() * 1.8f), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    //计算模型矩阵
    glm::mat4 Model = glm::mat4(1.0f);
    //先做缩放、再做旋转、再进行平移操作
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, -m_pModel->GetAdjustModelPosVec());
    m_ModelMatrix = Model;

    //整合MVP矩阵：投影矩阵 * 视图矩阵 * 模型矩阵
    mvpMatrix = Projection * View * Model;
}

void Model3DSample::initSceneResources() {
    //首先加载当前3D模型Obj文件，此处使用Mesh加载obj文件
    //obj文件里面已包含纹理贴图说明
    //app层已把model文件夹拷贝到 /sdcard/Android/data/com.chenxf.opengles/files/Download 路径下，所以这里可以加载模型
    std::string path(DEFAULT_OGL_ASSETS_DIR);

    DEBUG_LOGCATE();
    m_pModel = new Model(path + "/model/3Dmodel/model_4/nanosuit.obj");

    //加载当前Shader
    m_pShader = new Shader(m_pAssetManager,"shader/avata1/avata1_v.glsl","shader/avata1/avata1_f.glsl");
}