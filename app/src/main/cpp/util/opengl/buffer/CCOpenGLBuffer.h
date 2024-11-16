//
// Created by xuwen on 2022-07-21.
//

#ifndef OPENGLENGINE_CCOPENGLBUFFER_H
#define OPENGLENGINE_CCOPENGLBUFFER_H

#include "CCNDKCommonDef.h"

class CCOpenGLBuffer {

public:

    enum Type{
        VertexBuffer = GL_ARRAY_BUFFER, //VBO
        IndexBuffer = GL_ELEMENT_ARRAY_BUFFER,//EBO
        PixelPackBuffer = GL_PIXEL_PACK_BUFFER,//PBO
    };

    //数据对应的配置参数
    enum UsagePattern {
        StreamDraw = GL_STREAM_DRAW,
        StreamRead = GL_STREAM_READ,
        StreamCopy = GL_STREAM_COPY,
        StaticDraw = GL_STATIC_DRAW,
        StaticRead = GL_STATIC_READ,
        StaticCopy = GL_STATIC_COPY,
        DynamicDraw = GL_DYNAMIC_DRAW,
        DynamicRead = GL_DYNAMIC_READ,
        DynamicCopy = GL_DYNAMIC_COPY
    };

    CCOpenGLBuffer(CCOpenGLBuffer::Type type,CCOpenGLBuffer::UsagePattern usage);
    ~CCOpenGLBuffer();

    void Bind();//绑定当前buffer
    void Release();//解绑当前buffer

    void Create();//创建buffer
    GLuint GetID();//获取bufferId

    //向buffer传入数据
    void SetBufferData(const GLvoid *data,GLsizeiptr size);

private:

    CCOpenGLBuffer::Type m_bufferType;//当前buffer缓存对象类型
    GLuint  m_bufferId;//当前缓存id
    GLsizeiptr m_bufferSize; //当前缓存大小
    CCOpenGLBuffer::UsagePattern m_usage;//当前使用的模式

};


#endif //OPENGLENGINE_CCOPENGLBUFFER_H
