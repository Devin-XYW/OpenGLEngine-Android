//
// Created by xuwen on 2022-09-03.
//

#include "AudioDecoder.h"

int AudioDecoder::audioDecoderInit(const char *url) {
    strcpy(m_url,url);
    int result = -1;
    //1.创建封装格式上下文
    m_AVFormatContext = avformat_alloc_context();
    //2.打开文件
    if(avformat_open_input(&m_AVFormatContext,m_url, nullptr, nullptr) != 0){
        LOGE("avformat_open_input fail");
    }
    //3.获取音视频流信息
    if(avformat_find_stream_info(m_AVFormatContext, nullptr) < 0){
        LOGE("find avformat stream fail");
    }
    //4.获取音频流索引
    m_StreamIndex = av_find_best_stream(m_AVFormatContext,AVMEDIA_TYPE_AUDIO,-1,-1, nullptr,0);
    //5.获取解码器参数
    AVCodecParameters *codecParameters = m_AVFormatContext->streams[m_StreamIndex]->codecpar;
    //6.获取解码器
    m_AVCodec = avcodec_find_decoder(codecParameters->codec_id);
    if(m_AVCodec == nullptr){
        LOGE("find decoder failed");
    }
    //7.创建解码器上下文
    m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);
    //将流的参数复制到解码器中，否则某些流可能无法正常解码
    if(avcodec_parameters_to_context(m_AVCodecContext,codecParameters) != 0){
        LOGE("avcodec_parameters_to_context failed");
    }
    //8.打开解码器
    result = avcodec_open2(m_AVCodecContext,m_AVCodec,NULL);
    //计算音频流时长，单位从us变为ms
    m_Duration = m_AVFormatContext->duration / AV_TIME_BASE * 1000;
    //创建AVPacket存放编码数据
    m_Packet = av_packet_alloc();
    //创建AVFrame存放解码后的视频帧
    m_Frame = av_frame_alloc();
    return result;
}

int AudioDecoder::audioDecoderStart() {

    int result = av_read_frame(m_AVFormatContext,m_Packet);
    while(result >= 0){
        //过滤数据，找出对应的流，符合才进一步解码
        if(m_Packet->stream_index == m_StreamIndex){
            //解码音频，将packet数据交给avcodec进行解码
            if(avcodec_send_packet(m_AVCodecContext,m_Packet) != 0){
                return -1;
            }
            while(avcodec_receive_frame(m_AVCodecContext,m_Frame) == 0){
                //获取到m_Frame解码数据，在这里进行格式转换，然后进行渲染并播放
            }
        }
        av_packet_unref(m_Packet);
        result = av_read_frame(m_AVFormatContext,m_Packet);
    }
    return result;
}


void AudioDecoder::audioDecoderRelease() {
    //释放资源，解码完成
    if(m_Frame != nullptr){
        av_frame_free(&m_Frame);
        m_Frame = nullptr;
    }

    if(m_Packet != nullptr){
        av_packet_free(&m_Packet);
        m_Packet = nullptr;
    }

    if(m_AVCodecContext != nullptr){
        avcodec_close(m_AVCodecContext);
        avcodec_free_context(&m_AVCodecContext);
        m_AVCodecContext = nullptr;
        m_AVCodec = nullptr;
    }

    if(m_AVFormatContext != nullptr){
        avformat_close_input(&m_AVFormatContext);
        avformat_free_context(m_AVFormatContext);
        m_AVFormatContext = nullptr;
    }

}






