//
// Created by xuwen on 2022-07-21.
//

#include "CCOpenGLBuffer.h"

CCOpenGLBuffer::CCOpenGLBuffer(CCOpenGLBuffer::Type type, CCOpenGLBuffer::UsagePattern usage) {

    m_bufferSize = 0;
    m_usage = usage;
    m_bufferType = type;
}

CCOpenGLBuffer::~CCOpenGLBuffer() {
    //销毁对应的缓存
    glDeleteBuffers(1,&m_bufferId);
}

void CCOpenGLBuffer::Create() {
    //创建一个buffer，并获取id
    glGenBuffers(1,&m_bufferId);
}

GLuint CCOpenGLBuffer::GetID() {
    return m_bufferId;
}

void CCOpenGLBuffer::Bind() {
    //绑定当前buffer，传入对应的buffer类型，及id
    glBindBuffer(m_bufferType,m_bufferId);
}

void CCOpenGLBuffer::Release() {
    //解绑对应的buffer类型
    glBindBuffer(m_bufferType,0);
}

void CCOpenGLBuffer::SetBufferData(const void *data, GLsizeiptr size) {
   //向buffer中传入数据
   if(size > m_bufferSize){
       m_bufferSize = size;
       glBufferData(m_bufferType,size,data,m_usage);
   } else{
       glBufferSubData(m_bufferType,0,size,data);
   }

}








