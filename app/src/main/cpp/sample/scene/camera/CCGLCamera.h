//
// Created by xuwen on 2022-07-23.
//

#ifndef OPENGLENGINE_CCGLCAMERA_H
#define OPENGLENGINE_CCGLCAMERA_H

#include "CCNDKCommonDef.h"

class CCGLCamera {
public:
    explicit CCGLCamera();
    virtual ~CCGLCamera();

    //获取摄像机观察者位置
    glm::vec3 GetEyePosition(){
        return  m_eye;
    }

public:
    glm::mat4x4 viewMatrix;//观察矩阵
    glm::mat4x4 projectionMatrix;//投影矩阵

private:

    //摄像机位置
    glm::vec3 m_eye = glm::vec3(0,0,1);

    //摄像机观察点
    glm::vec3 m_center = glm::vec3(0,0,0);

    //摄像机上方向
    glm::vec3 m_up = glm::vec3(0,1,0);

};


#endif //OPENGLENGINE_CCGLCAMERA_H
