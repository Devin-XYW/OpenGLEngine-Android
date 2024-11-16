 //
// Created by chenchao on 2021/9/5.
//

#ifndef CCOPENCV_CCVIDEOWRITER_H
#define CCOPENCV_CCVIDEOWRITER_H


#pragma once

extern "C" {

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>

}

#include <pthread.h>
#include <iostream>
#include "faac.h"
#include "CCNDKLogDef.h"

using namespace std;



typedef struct MP4_AAC_CONFIGURE{
    faacEncHandle hEncoder;        //音频文件描述符
    unsigned int nSampleRate;     //音频采样数
    unsigned int nChannels;  	      //音频声道数
    unsigned int nPCMBitSize;        //音频采样精度
    unsigned int nInputSamples;      //每次调用编码时所应接收的原始数据长度
    unsigned int nMaxOutputBytes;    //每次调用编码时生成的AAC数据的最大长度
    unsigned char* pcmBuffer;       //pcm数据
    unsigned char* aacBuffer;       //aac数据

}AACEncodeConfig;

class CCVideoWriter
{

public:
    static CCVideoWriter* GetInstance();

private:
    static CCVideoWriter*   m_pInstance;
    static pthread_mutex_t  m_mutex;

    //视频输入参数
    int m_videoInWidth      = 640;
    int m_videoInHeight     = 360;

    int  m_videoInPixFmt    = AV_PIX_FMT_ARGB;

    //音频输入参数
    int m_audioInSamplerate = 22050;
    int m_audioInChannels   = 1;


    //视频输出参数
    int m_videoOutBitrate   = 512000;//比特率
    int m_videoOutWidth     = 640;
    int m_videoOutHeight    = 360;
    int m_videoOutFps       = 15;//帧率

    //音频输出参数
    int m_audioOutBitrate       = 64000;//比特率
    int m_audioOutSamplerate    = 44100;//采样率
    int m_audioOutChannels      = 2;//声道数

    int m_samples =960;//输入输出的每帧数据每通道的样本数

    //FFmpeg相关的结构体
    AVFormatContext *avFormatCtx = NULL;//封装mp4输出上下文

    AVCodecContext *videoCodecCtx = NULL;//视频编码器上下文
    AVCodecContext *audioCodecCtx = NULL;//音频编码器上下文

    AVStream* videoStream = NULL;//视频流
    AVStream* audioStream = NULL;//音频流

    AVFrame* yuvFrame = NULL;//输出yuv

    int m_audioFramePts = 0;//音频的pts;
    int m_videoFramePts=0;//视频的pts

    std::string m_filePath;//保存的文件路径

    bool    m_bRecording=false;//标志当前是否正在录制

    pthread_mutex_t     videoWriterMutex;//视频录制过程中的锁
    pthread_mutex_t     aacWriterMutex;//音频录制过程中的锁

    unsigned long                                    lastAudioTimeStamp;
    unsigned long                                    currentAudioTimeStamp;

    int                         m_pcmBufferSize=0;
    int                         m_pcmBufferRemainSize=0;
    int                         m_pcmWriteRemainSize=0;
    AACEncodeConfig*            g_aacEncodeConfig;

    unsigned long                startTimeStamp;

public:

    //开始录制
    bool StartRecordWithFilePath(const char* file) ;
    //停止录制并释放资源
    void StopRecordReleaseAllResources() ;

    //设置输入图片的宽高
    void setupInputResolution(int w,int h){ m_videoInWidth=w; m_videoInHeight=h;}
    //设置输出图片的宽高
    void setupOutputResolution(int w,int h){ m_videoOutWidth=w,m_videoOutHeight=h;}
    //设置视频帧率、比特率等
    void setupVideoOutParams(int fps,int bit ){m_videoOutFps=fps; m_videoOutBitrate=bit;}

    //写入rgb数据: RGB数据转换为YUV，再编码成H264
    bool WriteVideoFrameWithRgbData(const unsigned char* rgb) ;
    //写入pcm数据：PCM数据编码成AAC
    bool WriteAudioFrameWithPcmData(unsigned char* data,int captureSize);


private:

    CCVideoWriter();
    ~CCVideoWriter();

    bool AddVideoStream() ;//添加视频流
    bool AddAudioStream() ;//添加音频流

    //写入数据帧
    bool WriteFrame(AVPacket* pkt);

    //写入MP4头
    bool WriteVideoHeader() ;
    //写入MP4尾
    bool WriteMp4FileTrail() ;

    void ReleaseAllRecordResources();

    //初始化AAC参数
    AACEncodeConfig* initAudioEncodeConfiguration();
    void ReleaseAccConfiguration();

    int linearPCM2AAC(unsigned char * pData,int captureSize);

    unsigned long GetTickCount();

    //声明单例垃圾回收类
    class Garbage{
    public:
        ~Garbage(){
            if(CCVideoWriter::m_pInstance != nullptr){
                std::cout<<"Recycle CCVideoWriter::m_pInstance"<<endl;
                delete CCVideoWriter::m_pInstance;
                CCVideoWriter::m_pInstance= NULL;
            }
        }
    };

    //声明静态变量垃圾回收对象：在程序退出时也会调用静态成员变量
    static Garbage  m_garbage;
};

#endif //CCOPENCV_CCVIDEOWRITER_H
