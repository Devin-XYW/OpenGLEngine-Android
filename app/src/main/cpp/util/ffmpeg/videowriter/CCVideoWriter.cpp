//
// Created by chenchao on 2021/9/5.
//

#include "CCVideoWriter.h"

#include "libyuv.h"
#include<omp.h>
#include <sys/time.h>

//单例模式返回的单例变量
CCVideoWriter* CCVideoWriter::m_pInstance=NULL;
//声明单例对象创建时的mutex锁
pthread_mutex_t CCVideoWriter::m_mutex;
//创建单例垃圾回收对象
CCVideoWriter::Garbage CCVideoWriter::m_garbage;

CCVideoWriter* CCVideoWriter::GetInstance()
{

    if(m_pInstance == NULL){

        //创建锁
        pthread_mutex_init(&m_mutex,NULL);

        //下面创建进行加锁防止多线程情况下多次创建
        pthread_mutex_lock(&m_mutex);
        //创建单例对象
        m_pInstance= new CCVideoWriter();
        //解锁
        pthread_mutex_unlock(&m_mutex);
    }


    return m_pInstance;
}


CCVideoWriter::CCVideoWriter()
{

    g_aacEncodeConfig=NULL;

    av_register_all();
    avcodec_register_all();
    //初始化写视频和音频的锁
    pthread_mutex_init(&videoWriterMutex,NULL);
    pthread_mutex_init(&aacWriterMutex,NULL);

    g_aacEncodeConfig=initAudioEncodeConfiguration();

    startTimeStamp=0;

}

CCVideoWriter::~CCVideoWriter()
{
    ReleaseAccConfiguration();
}



void CCVideoWriter::StopRecordReleaseAllResources()
{

    pthread_mutex_lock(&videoWriterMutex);

    m_bRecording=false;

    WriteMp4FileTrail();

    ReleaseAllRecordResources();

    pthread_mutex_unlock(&videoWriterMutex);

}


void CCVideoWriter::ReleaseAllRecordResources()
{


    if (audioCodecCtx){
        avcodec_free_context(&audioCodecCtx);
    }

    if (videoCodecCtx){
        avcodec_free_context(&videoCodecCtx);
    }

    if (avFormatCtx){
        avformat_free_context(avFormatCtx);//内部会将ic置为NULL
    }

    if (yuvFrame){
        av_frame_free(&yuvFrame);
    }
}


bool CCVideoWriter::StartRecordWithFilePath(const char* file)
{
    yuvFrame = NULL;
    audioCodecCtx = NULL;
    videoCodecCtx = NULL;
    avFormatCtx = NULL;

    //申请AVFormatContext
    avformat_alloc_output_context2(&avFormatCtx, NULL, NULL, file);
    if (!avFormatCtx){
        LOGD("avformat_alloc_output_context2 failed!");
        return false;
    }

    m_filePath = file;

    m_videoFramePts=0;
    m_audioFramePts=0;

    //在AVFormatContext中添加视频流和音频流
    //添加视频流
    AddVideoStream();
    //添加音频Stream
    AddAudioStream();

    //写MP4的头
    if (!WriteVideoHeader()){
        //写MP4的尾
        WriteMp4FileTrail();
        ReleaseAllRecordResources();
        LOGD("WriteVideoHeader failed!");
        return false;
    }

    m_bRecording=true;

    startTimeStamp=GetTickCount();

    return true;
}

bool CCVideoWriter::AddVideoStream()
{
    if (!avFormatCtx) return false;
    //1.视频编码器创建：查找H264编码器
    AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec){
        LOGD("avcodec_find_encoder AV_CODEC_ID_H264 failed!");
        return false;
    }

    //2.获取编码器上下文
    videoCodecCtx = avcodec_alloc_context3(codec);
    if (!videoCodecCtx){
        LOGD("avcodec_alloc_context3 failed!");
        return false;
    }

    //3.设置编码器上下文信息
    //比特率，压缩后每秒大小
    videoCodecCtx->bit_rate = m_videoOutBitrate;
    videoCodecCtx->width = m_videoOutWidth;
    videoCodecCtx->height = m_videoOutHeight;
    //时间基数：跟帧率差不多
    videoCodecCtx->time_base = (AVRational){1,30};
    //设置GOP大小：画面组大小，多少帧一个关键帧
    videoCodecCtx->gop_size = 12;
    //B帧数设为0
    videoCodecCtx->max_b_frames = 0;
    videoCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    videoCodecCtx->codec_id = AV_CODEC_ID_H264;
    av_opt_set(videoCodecCtx->priv_data, "preset","superfast", 0);
    //设置每帧头都设置一定信息
    videoCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    //4.打开编码器
    int ret = avcodec_open2(videoCodecCtx, codec, NULL);
    if (ret != 0){
        avcodec_free_context(&videoCodecCtx);
        LOGD("avcodec_open2 failed!");
        return false;
    }
    LOGD("avcodec_open2 success!");

    //5.添加视频流到输出上下文
    videoStream = avformat_new_stream(avFormatCtx, NULL);
    videoStream->codecpar->codec_tag = 0;//流内部不包含编码器信息
    //在AVFormatContext中添加一个视频流
    avcodec_parameters_from_context(videoStream->codecpar, videoCodecCtx);

    av_dump_format(avFormatCtx, 0, m_filePath.c_str(), 1);



    if (!yuvFrame){
        //申请视频帧
        yuvFrame = av_frame_alloc();
        //设置视频帧信息
        yuvFrame->format = AV_PIX_FMT_YUV420P;
        yuvFrame->width = m_videoOutWidth;
        yuvFrame->height = m_videoOutHeight;
        yuvFrame->pts = 0;
        int ret = av_frame_get_buffer(yuvFrame, 0);
        if (ret != 0){
            LOGD("av_frame_get_buffer failed!");
            return false;
        }
    }


    return true;
}

bool CCVideoWriter::WriteVideoFrameWithRgbData(const unsigned char* rgb)
{

    if(!m_bRecording){
        return false;
    }
    if (!avFormatCtx || !yuvFrame) return false;

    if(rgb ==NULL){
        return false;
    }

    //输入图片的宽高
    int t_width = m_videoInWidth;
    int t_height = m_videoInHeight;

    int whSize = t_width * t_height;
    int rgbaSize = whSize * 4;//RGBA数据大小

    //申请YUV420P Buffer
    uint8_t * yuvBuffer= (uint8_t *) malloc(whSize * 3/2 );

    int uv_stride = t_width  / 2;
    //uv的数据大小
    int uv_length = uv_stride * t_height  / 2;

    uint8_t *Y_data_Dst = yuvBuffer;//Y分量的指针起始位置
    uint8_t *U_data_Dst = yuvBuffer + whSize;//U分量的指针起始位置
    uint8_t *V_data_Dst = U_data_Dst + uv_length;//V分量的指针起始位置

    //这里使用的是google提供的libyuv的库进行rgb转yuv
    libyuv::ABGRToI420((uint8_t *)rgb,rgbaSize, Y_data_Dst,t_width ,
                       U_data_Dst, uv_stride, V_data_Dst, uv_stride,
                       t_width, -t_height);

    if(!m_bRecording){
        return false;
    }
    if ( !yuvFrame) return false;

    yuvFrame->data[0] = Y_data_Dst;
    yuvFrame->data[1] = U_data_Dst;
    yuvFrame->data[2] = V_data_Dst;

    yuvFrame->linesize[0]= t_width;
    yuvFrame->linesize[1]=uv_stride;
    yuvFrame->linesize[2]=uv_stride;

    AVRational timeBase;
    timeBase.num = 1;
    timeBase.den = 1000;


    //获取当前PTS：当前时间 - 开始录制时间
    unsigned long currentPts = GetTickCount()-startTimeStamp;

    //保证PTS是不断递增
    if(currentPts <=0 ){
        LOGD("audio duration<=0:::::: %ld",currentPts);
        currentPts += 1;
    }
    //设置当前视频帧的PTS数据
    yuvFrame->pts=currentPts;

    if(!m_bRecording){
        return false;
    }
    if (!videoCodecCtx || !yuvFrame) return false;
    //encode：对YUV数据进行编码
    int ret = avcodec_send_frame(videoCodecCtx, yuvFrame);
    if (ret != 0){
        if(yuvBuffer != NULL){
            free(yuvBuffer);
            yuvBuffer = NULL;
        }
        return false;
    }

    if(!m_bRecording){
        return false;
    }
    if (!avFormatCtx || !yuvFrame){
        return false;
    }

    //申请一个AVPacket
    AVPacket packet;
    av_init_packet(&packet);
    if(!m_bRecording){
        return false;
    }
    if (!videoCodecCtx || !yuvFrame) return false;
    //将编码后的数据写入AVPacket
    ret = avcodec_receive_packet(videoCodecCtx, &packet);
    if (ret != 0 || packet.size <= 0){
        if(yuvBuffer != NULL){
            free(yuvBuffer);
            yuvBuffer = NULL;
        }
        return false;
    }

    if(yuvBuffer != NULL){
        free(yuvBuffer);
        yuvBuffer = NULL;
    }

    //设置packet的时间基
    av_packet_rescale_ts(&packet, videoCodecCtx->time_base, videoStream->time_base);
    packet.stream_index = videoStream->index;//视频流的索引

    //将packet写入封装
    WriteFrame(&packet);

    return true;
}

bool CCVideoWriter::WriteVideoHeader()
{
    if (!avFormatCtx) return false;
    //打开io
    int ret = avio_open(&avFormatCtx->pb, m_filePath.c_str(), AVIO_FLAG_WRITE);
    if (ret != 0){
        LOGD("avio_open failed! %s" ,m_filePath.c_str());
        return false;
    }
    //写入封装头
    ret = avformat_write_header(avFormatCtx, NULL);
    if (ret != 0){
        LOGD("avformat_write_header failed!" );
        return false;
    }
    LOGD("Write %s   success!" ,m_filePath.c_str() );

    return true;
}

bool CCVideoWriter::WriteFrame(AVPacket* pkt)
{
    if(!m_bRecording){
        return false;
    }
    if (!avFormatCtx || !pkt || pkt->size <= 0){
        return false;
    }
    if(pkt == NULL){
        return false;
    }
    if(pkt->data ==NULL){
        return false;
    }
    //写视频数据加锁
    pthread_mutex_lock(&videoWriterMutex);
    //向MP4容器中写AVPacket数据
    int retValue = av_interleaved_write_frame(avFormatCtx, pkt);
    pthread_mutex_unlock(&videoWriterMutex);
    //av_write_frame
    if (retValue != 0) {
        return false;
    }

    return true;
}

bool CCVideoWriter::WriteMp4FileTrail()
{
    if (!avFormatCtx || !avFormatCtx->pb) return false;
    //写入尾部信息索引
    if (av_write_trailer(avFormatCtx) != 0)
    {
        LOGD("av_write_trailer failed!");
        return false;
    }

    //关闭IO
    if (avio_closep(&avFormatCtx->pb) != 0){
        LOGD("avio_close failed!" );
        return false;
    }

    LOGD("WriteEnd success!" );

    return true;
}

bool CCVideoWriter::AddAudioStream()
{
    if (!avFormatCtx) return false;

    //1.找到音频编码: 找aac的编码器
    AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!codec){
        LOGD("avcodec_find_encoder AV_CODEC_ID_AAC failed!");
        return false;
    }

    //2.创建音频解码器上下文
    audioCodecCtx = avcodec_alloc_context3(codec);
    if (!audioCodecCtx){
        LOGD("avcodec_alloc_context3 failed!" );
        return false;
    }

    //3.设置Context上下文信息
//    audioCodecCtx->bit_rate = 256000;//帧率
    audioCodecCtx->sample_rate = m_audioOutSamplerate;//采样率
    audioCodecCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;//16位
    audioCodecCtx->channels = m_audioOutChannels;
    audioCodecCtx->channel_layout = av_get_default_channel_layout(m_audioOutChannels);
    audioCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;//设置全局头

    //4.打开编码器
    int ret = avcodec_open2(audioCodecCtx, codec, NULL);
    if (ret != 0){
        avcodec_free_context(&audioCodecCtx);
        LOGD("audio avcodec_open2 failed!" );
        return false;
    }
    //3.在AVFormatContext中新增音频流
    audioStream = avformat_new_stream(avFormatCtx, NULL);
    if (!audioStream){
        LOGD("avformat_new_stream failed" );
        return false;
    }
    audioStream->codecpar->codec_tag = 0;
    //设置编码器参数信息
    avcodec_parameters_from_context(audioStream->codecpar, audioCodecCtx);
    av_dump_format(avFormatCtx, 0, m_filePath.c_str(), 1);

    return true;
}

void CCVideoWriter::ReleaseAccConfiguration()
{
    if(g_aacEncodeConfig ==NULL)
    {
        return;
    }

    pthread_mutex_lock(&aacWriterMutex);

    if(g_aacEncodeConfig->hEncoder!=NULL)
    {
        faacEncClose(g_aacEncodeConfig->hEncoder);
        g_aacEncodeConfig->hEncoder = NULL;
    }

    if(g_aacEncodeConfig->pcmBuffer!=NULL)
    {
        free(g_aacEncodeConfig->pcmBuffer);
        g_aacEncodeConfig->pcmBuffer = NULL;
    }

    if(g_aacEncodeConfig->aacBuffer!=NULL)
    {
        free(g_aacEncodeConfig->aacBuffer);
        g_aacEncodeConfig->aacBuffer = NULL;
    }
    if(g_aacEncodeConfig ==NULL)
    {
        free(g_aacEncodeConfig);
        g_aacEncodeConfig=NULL;
    }

    pthread_mutex_unlock(&aacWriterMutex);

}

bool CCVideoWriter::WriteAudioFrameWithPcmData(unsigned char* audioData,int captureSize)
{
    if(!m_bRecording){
        return false;
    }

    linearPCM2AAC(audioData,captureSize);

    return true;
}


AACEncodeConfig* CCVideoWriter::initAudioEncodeConfiguration()
{
    AACEncodeConfig* aacConfig = NULL;

    faacEncConfigurationPtr pConfiguration;

    int nRet = 0;
    m_pcmBufferSize = 0;

    aacConfig = (AACEncodeConfig*)malloc(sizeof(AACEncodeConfig));

    aacConfig->nSampleRate = m_audioOutSamplerate;
    aacConfig->nChannels = 1;
    aacConfig->nPCMBitSize = 16;
    aacConfig->nInputSamples = 0;
    aacConfig->nMaxOutputBytes = 0;

    aacConfig->hEncoder = faacEncOpen(aacConfig->nSampleRate, aacConfig->nChannels,  (unsigned long *)&aacConfig->nInputSamples, (unsigned long *)&aacConfig->nMaxOutputBytes);
    if(aacConfig->hEncoder == NULL){
        LOGD("failed to call faacEncOpen()");
        return NULL;
    }

    m_pcmBufferSize = (int)(aacConfig->nInputSamples*(aacConfig->nPCMBitSize/8));
    m_pcmBufferRemainSize=m_pcmBufferSize;

    aacConfig->pcmBuffer=(unsigned char*)malloc(m_pcmBufferSize*sizeof(unsigned char));
    memset(aacConfig->pcmBuffer, 0, m_pcmBufferSize);

    aacConfig->aacBuffer=(unsigned char*)malloc(aacConfig->nMaxOutputBytes*sizeof(unsigned char));
    memset(aacConfig->aacBuffer, 0, aacConfig->nMaxOutputBytes);


    pConfiguration = faacEncGetCurrentConfiguration(aacConfig->hEncoder);

    pConfiguration->inputFormat = FAAC_INPUT_16BIT;
    pConfiguration->outputFormat = 0;
    pConfiguration->aacObjectType = LOW;


    nRet = faacEncSetConfiguration(aacConfig->hEncoder, pConfiguration);

    return aacConfig;
}


int CCVideoWriter::linearPCM2AAC(unsigned char * pData,int captureSize)
{
    if(!m_bRecording){
        return -1;
    }

    if(pData==NULL){
        return -1;
    }

    if((captureSize>m_pcmBufferSize)||(captureSize<=0)){
        return -1;
    }

    int nRet = 0;
    int copyLength = 0;

    if(m_pcmBufferRemainSize > captureSize){
        copyLength = captureSize;
    }
    else{
        copyLength = m_pcmBufferRemainSize;
    }

    memcpy((&g_aacEncodeConfig->pcmBuffer[0]) + m_pcmWriteRemainSize, pData, copyLength);
    m_pcmBufferRemainSize -= copyLength;
    m_pcmWriteRemainSize += copyLength;

    if(m_pcmBufferRemainSize > 0){
        return 0;
    }

    pthread_mutex_lock(&aacWriterMutex);

    nRet = faacEncEncode(g_aacEncodeConfig->hEncoder,(int*)(g_aacEncodeConfig->pcmBuffer),g_aacEncodeConfig->nInputSamples,g_aacEncodeConfig->aacBuffer,g_aacEncodeConfig->nMaxOutputBytes);

    pthread_mutex_unlock(&aacWriterMutex);

    memset(g_aacEncodeConfig->pcmBuffer, 0, m_pcmBufferSize);
    m_pcmWriteRemainSize = 0;
    m_pcmBufferRemainSize = m_pcmBufferSize;


    AVPacket pkt;
    av_init_packet(&pkt);

    pkt.stream_index = audioStream->index;//音频流的索引
    pkt.data=g_aacEncodeConfig->aacBuffer;
    pkt.size=nRet;
    pkt.pts = m_audioFramePts;
    pkt.dts = pkt.pts;
    AVRational rat=(AVRational){1,audioCodecCtx->sample_rate};

    m_audioFramePts += av_rescale_q(m_samples, rat, audioCodecCtx->time_base);

    WriteFrame(&pkt);

    memset(g_aacEncodeConfig->pcmBuffer, 0, m_pcmBufferSize);
    if((captureSize - copyLength) > 0 ){
        memcpy((&g_aacEncodeConfig->pcmBuffer[0]), pData+copyLength, captureSize - copyLength);
        m_pcmWriteRemainSize = captureSize - copyLength;
        m_pcmBufferRemainSize = m_pcmBufferSize - (captureSize - copyLength);
    }

    return nRet;

}

unsigned long CCVideoWriter::GetTickCount()
{

    //获取当前时间，单位为毫秒
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
        return 0;

    return (tv.tv_sec*1000  + tv.tv_usec / 1000);
}