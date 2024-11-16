//
// Created by xuwen on 2022-07-20.
//

#include "CCOpenGLShader.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

CCOpenGLShader::CCOpenGLShader() {
    m_shaderProgram = 0;
}

CCOpenGLShader::~CCOpenGLShader() {

}

//创建并初始化assets目录下的shader文件
void CCOpenGLShader::InitShaderFromFile(AAssetManager *pManager, const char *vShader,
                                        const char *fShader) {
    GLuint vertexId = 0;
    GLuint fragId = 0;

    //初始化并编译顶点着色器，其中GL_VERTEX_SHADER表示当前编译类型为顶点着色器
    vertexId = compileShader(pManager,vShader,GL_VERTEX_SHADER);

    //初始化并编译片段着色器，其中GL_FRAGMENT_SHADER表示当前编译类型为片段着色器
    fragId = compileShader(pManager,fShader,GL_FRAGMENT_SHADER);

    char message[512];//记录log字段信息
    int status = 0;

    //创建一个着色器程序
    m_shaderProgram = glCreateProgram();

    if(vertexId != -1){
        //attach着色器程序和顶点着色器Id
        glAttachShader(m_shaderProgram,vertexId);
    }

    if(fragId != -1){
        //attach着色器程序和片段着色器
        glAttachShader(m_shaderProgram,fragId);
    }

    //链接着色器程序
    glLinkProgram(m_shaderProgram);

    //检查当前程序是否链接成功
    glGetProgramiv(m_shaderProgram,GL_LINK_STATUS,&status);
    if(!status){
        //如果没有链接成功则获取错误log信息
        glGetProgramInfoLog(m_shaderProgram,512,NULL,message);
        LOGF("Get shaderProgram failed: %s",message);
    }

    LOGI("ShaderProgram success !!!\n");

    //删除顶点着色器
    glDeleteShader(vertexId);

    //删除片段着色器
    glDeleteShader(fragId);
}

//编译shader程序,并返回编译成功的shaderId
int CCOpenGLShader::compileShader(AAssetManager *mAssetManager, const char *sPath, int sType) {

    //打开assets目录下的sPath路径文件
    AAsset* file = AAssetManager_open(mAssetManager,sPath,AASSET_MODE_BUFFER);
    size_t shaderSize = AAsset_getLength(file);

    //创建对应shader内容大小的Buffer
    char* sContentBuffer = (char*)malloc(shaderSize);
    //读取assets file中内容到sContentBuffer中
    AAsset_read(file,sContentBuffer,shaderSize);
    LOGD("SHADERS: %s",sContentBuffer);

    unsigned int shaderId = 0;
    char message[512] = {0};
    int status = 0;

    //根据当前着色器type创建shader，返回对应的shaderId
    shaderId = glCreateShader(sType);
    //写入file中读取的shader文件buffer内容
    glShaderSource(shaderId,1,&sContentBuffer,(const GLint*) &shaderSize);
    //编译着色器
    glCompileShader(shaderId);

    //检查当前shader是否编译成功
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    if (!status){
        //shader编译不成功则获取并打印对应的失败信息
        glGetShaderInfoLog(shaderId, 512, NULL, message);
        LOGF("Compile Shader Status failed: %s",message);
    }

    //编译成功后对当前申请的buffer进行释放
    if(sContentBuffer != NULL){
        free(sContentBuffer);
        sContentBuffer = NULL;
    }

    //关闭文件
    AAsset_close(file);

    return shaderId;
}

//绑定：使用当前着色器程序
void CCOpenGLShader::Bind(){
    glUseProgram(m_shaderProgram);
}

void CCOpenGLShader::Release() {
    glUseProgram(0);
}

void CCOpenGLShader::SetUniformValue(const char *name, int iValue) {
    glUniform1i(glGetUniformLocation(m_shaderProgram,name),iValue);
}

void CCOpenGLShader::SetUniformValue(const char* name, GLfloat fValue)
{
    glUniform1f(glGetUniformLocation(m_shaderProgram, name), fValue);
}

void CCOpenGLShader::SetUniformValue(const char* name, glm::vec2 vec2Value)
{
    glUniform2fv(glGetUniformLocation(m_shaderProgram, name), 1, (const GLfloat *)glm::value_ptr(vec2Value));
}

void CCOpenGLShader::SetUniformValue(const char* name, glm::vec3 vec3Value)
{
    glUniform3fv(glGetUniformLocation(m_shaderProgram, name), 1, (const GLfloat *)glm::value_ptr(vec3Value));
}

void CCOpenGLShader::SetUniformValue(const char* name, glm::mat4 matValue)
{
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, name) , 1 , GL_FALSE , glm::value_ptr(matValue));
}

void CCOpenGLShader::EnableAttributeArray(const char *name) {
    //首先获取变量名对应的locationId
    GLuint  location = glGetAttribLocation(m_shaderProgram,name);
    //开启对应locationId对应的变量
    glEnableVertexAttribArray(location);
}

void CCOpenGLShader::DisableAttributeArray(const char *name) {
    //首先获取变量名对应的locationId
    GLuint location = glGetAttribLocation(m_shaderProgram,name);
    //关闭对应locationId对应的变量
    glDisableVertexAttribArray(location);
}

void CCOpenGLShader::SetAttributeBuffer(const char *name, GLenum type, const void *values,
                                        int tupleSize, int stride) {

    //获取变量名对应的location
    GLuint location = glGetAttribLocation(m_shaderProgram,name);
    //设置locationId对应的变量的值
    glVertexAttribPointer(location,tupleSize,type,GL_FALSE,stride,values);
}

void CCOpenGLShader::EnableAttributeArray(int location)
{
    //开启locationId对应的变量
    glEnableVertexAttribArray(location);
}

void CCOpenGLShader::DisableAttributeArray(int location)
{
    //关闭Location对应的变量
    glDisableVertexAttribArray(location);
}


void CCOpenGLShader::SetAttributeBuffer(int location,GLenum type, const void *values, int tupleSize, int stride )
{
    //设置location变量对应的变量值
    glVertexAttribPointer(location,tupleSize,type,GL_FALSE,stride,values);
}















