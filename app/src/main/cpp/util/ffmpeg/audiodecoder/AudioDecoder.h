//
// Created by xuwen on 2022-09-03.
//

#ifndef OPENGLENGINE_AUDIODECODER_H
#define OPENGLENGINE_AUDIODECODER_H
extern "C" {

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>

}

#include <pthread.h>
#include <iostream>
#include "CCNDKLogDef.h"

class AudioDecoder {

public:

    AudioDecoder();
    ~AudioDecoder();

    int audioDecoderInit(const char *url);
    int audioDecoderStart();
    void audioDecoderRelease();

private:
    char* m_url;

    //封装格式上下文：这个结构体描述了一个媒体文件或者媒体流的构成和基本信息
    AVFormatContext* m_AVFormatContext;
    //音频流索引index
    int m_StreamIndex;
    //解码器上下文: 编码和解码时必须用到的结构体，
    //包含解码类型、视频宽高、音频通道数和采样信息等信息
    AVCodecContext* m_AVCodecContext;
    //视频解码器：存放编解码信息的结构体
    AVCodec* m_AVCodec;
    //视频流持续时间
    int64_t m_Duration;
    //解封装后的数据AVPacket
    AVPacket* m_Packet;
    //解码后的的数据帧AVFrame（原始数据）
    AVFrame* m_Frame;



};


#endif //OPENGLENGINE_AUDIODECODER_H
