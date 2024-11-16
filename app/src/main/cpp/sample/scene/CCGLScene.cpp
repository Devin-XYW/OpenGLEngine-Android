//
// Created by xuwen on 2022-07-23.
//

#include "CCGLScene.h"
#include <sys/time.h>

CCGLScene::CCGLScene() {

    m_directoryPath = "";
    m_bRecording = false;

    m_pCamera   = new CCGLCamera();//初始化摄像机
    m_pVideoRender = new CCGLVideoRender();//初始化视频画面
//    m_pTexturedCube = new CCTextureCube();//初始化立方体
    m_pSticker = new CCGLSticker();
    m_pModelLoader = new CCModelLoader();

    //初始化FBO
    m_pFBO = new CCOpenGLFBO();
    m_pTextureRender = new CCTextureRender();

    //声明两个PBO，分别交替进行数据读取和处理，从FBO中读取数据
    m_PBOfst = new CCOpenGLBuffer(CCOpenGLBuffer::PixelPackBuffer,CCOpenGLBuffer::StreamRead);
    m_PBOsnd = new CCOpenGLBuffer(CCOpenGLBuffer::PixelPackBuffer,CCOpenGLBuffer::StreamRead);
}

CCGLScene::~CCGLScene() {
    CCGLSafePtrDelete(m_pVideoRender);//释放视频画面
    CCGLSafePtrDelete(m_pCamera);//释放摄像机
//    CCGLSafePtrDelete(m_pTexturedCube);
    CCGLSafePtrDelete(m_pSticker);
    CCGLSafePtrDelete(m_pModelLoader);

    CCGLSafePtrDelete(m_pFBO);
    CCGLSafePtrDelete(m_pTextureRender);

    CCGLSafePtrDelete(m_PBOfst);
    CCGLSafePtrDelete(m_PBOsnd);
}

void CCGLScene::InitGL() {
    //清屏
    glClearColor(0.0,0.0,0.0,1.0);
    glClearDepthf(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //初始化当前场景需要的一些资源
    initSceneResources();

    getRenderScreenSize();
    //创建FBO帧缓冲对象
    setupFrameBufferObject();

    m_PBOfst->Bind();
    m_PBOfst->SetBufferData(NULL,m_nResolution.x * m_nResolution.y * 4);
    m_PBOfst->Release();

    m_PBOsnd->Bind();
    m_PBOsnd->SetBufferData(NULL,m_nResolution.x * m_nResolution.y * 4);
    m_PBOsnd->Release();

}

void CCGLScene::initSceneResources() {
    //初始化视频画面资源
    m_pVideoRender->InitRenderResources(m_pAssetManager);

//    //初始化3D场景中的立方体
//    m_pTexturedCube->InitRenderResources(m_pAssetManager);

    //初始化3D场景中的贴图
    m_pSticker->InitRenderResource(m_pAssetManager);
    //初始化3D场景中的3D模型
    m_pModelLoader->InitRenderResources(m_pAssetManager,m_directoryPath);

    m_pTextureRender->InitRenderResources(m_pAssetManager);

}

void CCGLScene::PaintGL() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pFBO->Bind();

    //配置视频画面相关Render
    m_pVideoRender->Render(m_pCamera);
//    //配置立方体相关Render
//    m_pTexturedCube->Render(m_pCamera);
    //配置贴图相关Render
//    m_pSticker->Render(m_pCamera);
//    //向贴图传递当前人脸移动相关信息：位置变换及缩放
//    m_pSticker->UpdateTrackingInfo(m_pCamera,m_trkPos,m_trkScale);
    //配置3D模型相关Render
    m_pModelLoader->Render(m_pCamera);
    //向3D模型传递人脸标定相关坐标数据
    m_pModelLoader->UpdateTracking(m_pCamera,m_trkPos,m_trkScale);

    if(m_bRecording){
        processPBOReadPixels();
    }

    m_pFBO->Release();

    m_pTextureRender->Render(m_pCamera,m_pFBO->GetTextureId());
}

void CCGLScene::processPBOReadPixels() {
    glReadBuffer(GL_FRONT);
    //PBOfst和PBOsnd进行交替读取和处理数据
    if(m_currIdx == 0){
        readPixelsFromPBO(m_PBOfst,m_PBOsnd);
        m_currIdx = 1;
    } else{
        readPixelsFromPBO(m_PBOsnd,m_PBOfst);
        m_currIdx = 0;
    }

}

void CCGLScene::readPixelsFromPBO(CCOpenGLBuffer *fstBuff, CCOpenGLBuffer *sndBuff) {

    int w = m_nResolution.x;
    int h = m_nResolution.y;

    //DMA:从帧缓冲区中读取数据
    fstBuff->Bind();
    //glReadPixels从OpenGL帧缓冲区中读取数据到PBO中
    glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,0);

    //READ：读取PBO中的数据
    sndBuff->Bind();
    void *data = glMapBufferRange(GL_PIXEL_PACK_BUFFER,0,w*h*4,GL_MAP_READ_BIT);
    if(data){
        CCVideoWriter::GetInstance()->WriteVideoFrameWithRgbData((const unsigned char*)data);
    }
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void CCGLScene::ResizeGL(int width, int height) {
    glViewport(0,0,width,height);
}

void CCGLScene::Release() {
    CCGLSafePtrDelete(m_pVideoRender);//释放视频画面
    CCGLSafePtrDelete(m_pCamera);//释放摄像机

    CCGLSafePtrDelete(m_pSticker);
    CCGLSafePtrDelete(m_pModelLoader);

    CCGLSafePtrDelete(m_pFBO);
    CCGLSafePtrDelete(m_pTextureRender);

    CCGLSafePtrDelete(m_PBOfst);
    CCGLSafePtrDelete(m_PBOsnd);
}

void CCGLScene::RenderVideo(YUVData_Frame *frame) {
    //将当前手机采集到的YUV图像帧数据传递到视频Render中进行渲染
    m_pVideoRender->UpdateYUVData(frame);
}

void CCGLScene::RecordVideo(bool recordState) {
    int w = m_nResolution.x;
    int h = m_nResolution.y;
    CCVideoWriter::GetInstance()->setupInputResolution(w,h);
    CCVideoWriter::GetInstance()->setupOutputResolution(w,h);

    std::string filePath = "/storage/emulated/0/Movies/"+std::to_string(getCurrentMSeconds())+".mp4";

    if(recordState == true){

       CCVideoWriter::GetInstance()->StartRecordWithFilePath(filePath.c_str());
        m_bRecording = true;
    }else{

        m_bRecording = false;
        CCVideoWriter::GetInstance()->StopRecordReleaseAllResources();
    }
}

void CCGLScene::SetupAssetManager(AAssetManager *assetManager, std::string path) {
    m_pAssetManager = assetManager;
    m_directoryPath = path;
}

void CCGLScene::UpdateTrackingInfo(glm::vec2 pos, float scale) {
    m_trkPos = pos;
    m_trkScale = scale;
}

void CCGLScene::getRenderScreenSize() {
    int viewport[4]= {0};
    glGetIntegerv( GL_VIEWPORT, viewport );
    int sWidth = viewport[2];
    int sHeight = viewport[3];
    m_nResolution = glm::vec2(sWidth,sHeight);

    LOGD("VIEWPORT: %d %d",viewport[2],viewport[3]);
}

void CCGLScene::setupFrameBufferObject() {
    if(m_nResolution.x > 0 && m_nResolution.y >0){
        m_pFBO->CreateWithSize(m_nResolution.x,m_nResolution.y);
    }
}

long long CCGLScene::getCurrentMSeconds(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}