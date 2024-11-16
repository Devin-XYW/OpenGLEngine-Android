//
// Created by xuwen on 2022-09-03.
//

#include "VideoDecoder.h"

int VideoDecoder::videoDecoderInit(const char *url) {
    strcpy(m_url,url);
    int result = -1;

    //注册容器和编码器等
    av_register_all();

    //1.创建封装格式上下文
    m_AVFormatContext = avformat_alloc_context();
    //2.打开文件，并获取文件全局上下文
    if(avformat_open_input(&m_AVFormatContext,m_url, nullptr, nullptr) != 0){
        LOGE("avformat_open_input fail");
    }
    //3.获取音视频流信息,对文件进行探测流信息
    if(avformat_find_stream_info(m_AVFormatContext, nullptr) < 0){
        LOGE("avformat_find_stream_info fail");
    }
    //4.获取视频流索引: AVMEDIA_TYPE_VIDEO表明获取视频流
    m_StreamIndex = av_find_best_stream(m_AVFormatContext,AVMEDIA_TYPE_VIDEO,-1,-1, nullptr,0);

    //5.获取解码器参数
    AVCodecParameters *codecParameters = m_AVFormatContext->streams[m_StreamIndex]->codecpar;
    //6.获取解码器
    m_AVCodec = avcodec_find_decoder(codecParameters->codec_id);
    if(m_AVCodec == nullptr){
        LOGE("get AVCodec failed");
    }

    //7.创建解码器上下文
    m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);
    //将流的参数（codecParameters）复制到解码器中，否则某些流可能无法正常解码
    if(avcodec_parameters_to_context(m_AVCodecContext,codecParameters) != 0){
        LOGE("get AvCodecContext failed");
    }

    //8.打开解码器
    result = avcodec_open2(m_AVCodecContext,m_AVCodec, NULL);
    if(result < 0){
        LOGE("AVCodec open failed");
    }

    //计算视频流时长，单位从us变为ms
    m_Duration = m_AVFormatContext->duration / AV_TIME_BASE * 1000;
    //创建AVPacket存放编码数据
    m_Packet = av_packet_alloc();
    //创建AVFrame存放解码后的视频帧
    m_Frame = av_frame_alloc();
    return result;
}

int VideoDecoder::videoDecoderStart() {

    //读取一帧packet数据包
    int result = av_read_frame(m_AVFormatContext,m_Packet);
    while(result >= 0){
        //过滤数据，找出对应流，符合才进一步进行解码
        if(m_Packet->stream_index == m_StreamIndex){
            //解码视频，将packet数据交给AVcodec处理
            if(avcodec_send_packet(m_AVCodecContext,m_Packet) != 0){
                return -1;
            }
            while(avcodec_receive_frame(m_AVCodecContext,m_Frame) == 0){
                //此处获取到解码后的数据，进行格式转换并进行渲染显示
                LOGD("m_Frame pts is %d",m_Frame->pts);
            }
        }
        av_packet_unref(m_Packet);
        result = av_read_frame(m_AVFormatContext,m_Packet);
    }
    return result;
}

void VideoDecoder::videoDecoderRelease() {
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













