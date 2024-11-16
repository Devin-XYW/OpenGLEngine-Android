//
// Created by xuwen on 2022-07-21.
//

#include "CCOpenGLVAO.h"

CCOpenGLVAO::CCOpenGLVAO() {

}

CCOpenGLVAO::~CCOpenGLVAO() {
    glDeleteVertexArrays(1,&m_vaoId);
}

void CCOpenGLVAO::Create() {
    glGenVertexArrays(1,&m_vaoId);
}

void CCOpenGLVAO::Bind(){
    glBindVertexArray(m_vaoId);
}

void CCOpenGLVAO::Release() {
    glBindVertexArray(0);
}