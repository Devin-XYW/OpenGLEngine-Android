//
// Created by xuwen on 2022-07-23.
//

#ifndef OPENGLENGINE_CCGLSCENE_H
#define OPENGLENGINE_CCGLSCENE_H

#include "CCGLCamera.h"
#include "CCGLVideoRender.h"
#include "CCTextureCube.h"
#include "CCGLSticker.h"
#include "CCVideoWriter.h"
#include "GLBaseSample.h"

class CCGLScene: public GLBaseSample{
public:
    CCGLScene();
    ~CCGLScene();

    //初始化当前AssetsManager
    void SetupAssetManager(AAssetManager *assetManager,std::string path);

    void InitGL();//SurfaceView创建时进行初始化
    void PaintGL();//渲染相关图像
    void ResizeGL(int width,int height);//SurfaceView画面改变时调用
    void Release();

    void RenderVideo(YUVData_Frame *frame);//将yuv数据传递进行视频渲染
    void RecordVideo(bool state);//记录Video数据，封装成MP4

    void UpdateTrackingInfo(glm::vec2 pos,float scale);

private:
    void initSceneResources();//初始化资源

    void getRenderScreenSize();
    void setupFrameBufferObject();

    void processPBOReadPixels();//PBO读取FBO中画面信息
    void readPixelsFromPBO(CCOpenGLBuffer* fstBuff, CCOpenGLBuffer* sndBuff);
    long long getCurrentMSeconds();

private:
    std::string m_directoryPath;//当前assets文件目录路径
    AAssetManager* m_pAssetManager;

    glm::vec2 m_nResolution;//记录当前屏幕宽高

    glm::vec2           m_trkPos;//人脸位置变换位置信息
    float               m_trkScale;//人脸变换缩放信息

    CCGLCamera* m_pCamera;//3D场景中的摄像机观察者
    CCGLVideoRender* m_pVideoRender;//3D中此时的视频画面
    CCTextureCube* m_pTexturedCube;//3D中增加的立方体
    CCGLSticker* m_pSticker;//3D中增加贴图
    CCModelLoader* m_pModelLoader;//场景中增加3D模型

    CCOpenGLFBO* m_pFBO;//FBO对象
    CCTextureRender* m_pTextureRender;//FBO数据显示到一个纹理画面上

    //声明两个PBO数据读取缓冲区
    //这里使用两个PBO的原因是实现读取数据和处理数据可以同步进行
    CCOpenGLBuffer*       m_PBOfst;
    CCOpenGLBuffer*       m_PBOsnd;

    int                   m_currIdx = 0;
    bool                  m_bRecording = false;
};


#endif //OPENGLENGINE_CCGLSCENE_H
