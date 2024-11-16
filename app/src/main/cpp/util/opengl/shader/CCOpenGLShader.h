//
// Created by xuwen on 2022-07-20.
//

#ifndef OPENGLENGINE_CCOPENGLSHADER_H
#define OPENGLENGINE_CCOPENGLSHADER_H

#include "CCNDKCommonDef.h"

class CCOpenGLShader {
public:
    CCOpenGLShader();
    ~CCOpenGLShader();

    void Bind();//绑定,使用当前着色器程序
    void Release();//释放，与Bind配套使用

    //从assets目录下加载对应的shader文件
    void InitShaderFromFile(AAssetManager* pManager, const char* vShader, const char* fShader);

    //向shader某值变量传递参数
    void DisableAttributeArray(const char *name);
    void EnableAttributeArray(const char *name);
    void SetAttributeBuffer(const char* name,GLenum type, const void *values,int tupleSize,int stride = 0);

    //向shader中某location值声明的变量传递参数
    void DisableAttributeArray(int location);
    void EnableAttributeArray(int location);
    void SetAttributeBuffer(int location,GLenum type, const void *values, int tupleSize, int stride = 0);

    //向shader中uniform值传递参数
    void SetUniformValue(const char* name, int iValue);
    void SetUniformValue(const char* name, GLfloat fValue);
    void SetUniformValue(const char* name, glm::vec2 vecValue);
    void SetUniformValue(const char* name, glm::vec3 vecValue);
    void SetUniformValue(const char* name, glm::mat4 matValue);

private:
    //编译Shader
    int compileShader(AAssetManager* mAssetManager, const char* sPath,int sType);

private:
    //当前shader程序id
    GLuint m_shaderProgram;
};


#endif //OPENGLENGINE_CCOPENGLSHADER_H
