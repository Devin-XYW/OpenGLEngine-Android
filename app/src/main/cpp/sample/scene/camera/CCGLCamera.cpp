//
// Created by xuwen on 2022-07-23.
//

#include "CCGLCamera.h"

CCGLCamera::CCGLCamera() {

    //声明观察矩阵
    viewMatrix = glm::mat4x4(1.0);
    viewMatrix = glm::lookAt(m_eye,m_center,m_up);

    //声明投影矩阵：为正交投影
    projectionMatrix = glm::mat4x4(1.0);
    projectionMatrix = glm::ortho(-1.0,1.0,-1.0,1.0,0.1,1000.0);

}

CCGLCamera::~CCGLCamera(){

}