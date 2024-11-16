//
// Created by xuwen on 2022-07-21.
//

#include "CCOpenGLFBO.h"

CCOpenGLFBO::CCOpenGLFBO() {
}

CCOpenGLFBO::~CCOpenGLFBO() {
    glDeleteTextures(1,&m_textureId);
    glDeleteRenderbuffers(1,&m_depthBufferId);
    glDeleteFramebuffers(1,&m_fboId);
}

void CCOpenGLFBO::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER,m_fboId);
}

void CCOpenGLFBO::Release(){
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

GLuint CCOpenGLFBO::GetTextureId() {
    return m_textureId;
}

void CCOpenGLFBO::CreateWithSize(int width, int height) {

    //创建纹理texture
    glGenTextures(1,&m_textureId);
    glBindTexture(GL_TEXTURE_2D,m_textureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //绑定texture
    glBindTexture(GL_TEXTURE_2D,0);

    //创建FBO并绑定texture
    glGenFramebuffers(1,&m_fboId);
    glBindFramebuffer(GL_FRAMEBUFFER,m_fboId);
    glBindTexture(GL_TEXTURE_2D,m_textureId);

    //将纹理图像附加到帧缓冲对象
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,m_textureId,0);
    //生成一个2D纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,width,height,0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    //创建深度或深度/模板renderbuffer
    glGenRenderbuffers(1, &m_depthBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width,height);

    //将其附加到framebuffer的深度附件点
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBufferId);

    GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER);
    if(status == GL_FRAMEBUFFER_COMPLETE) {
        LOGD("Framebuffer creation successful\n");
    } else {
        LOGF("Error creating framebuffer [status: %d]\n", status);
    }

    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}