#include <jni.h>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc.hpp>

#include "CCNDKLogDef.h"
#include "CCYUVDataDefine.h"
#include "CCNDKCommonDef.h"

#include "trackerexport.h"

#include "CCGLVideoRender.h"

#include "GLBaseSample.h"
#include "CCGLScene.h"
#include "CCSampleTexture2D.h"
#include "CCSampleTexture2DScale.h"
#include "CCTranstionTexture.h"
#include "CCGreenScreenTexture.h"
#include "CCGLCamera2D.h"
#include "Model3DSample.h"
#include "Model3DAnimSample.h"
#include "GL3DModel.h"
GLBaseSample *m_glSample;
int m_sample_type = 100;

extern "C"
JNIEXPORT void JNICALL
Java_com_xuwen_openglengine_render_CCOpenGLRender_ndkInitGL(JNIEnv *env, jobject thiz,
                                                     jobject assetManager, jstring mPath) {

    AAssetManager *astManager = AAssetManager_fromJava(env,assetManager);
    if(NULL != astManager){
        const char* dirPath = env->GetStringUTFChars(mPath,0);
        std::string pathDirString = std::string(dirPath);
        //传递当前assetManager及当前手机模型图片目录路径
        m_glSample->SetupAssetManager(astManager,pathDirString);

        if(m_sample_type == SAMPLE_TYPE_KEY_3D_CAMERA
            || m_sample_type == SAMPLE_TYPE_CAMERA){
            //获取当前手机存放OpenCV人脸标定模型文件的目录
            std::string modelPath = pathDirString + "/roboman-landmark-model.bin";
            std::string classiPath = pathDirString + "/haar_roboman_ff_alt2.xml";
            //OpenCV人脸标定库加载模型文件
            bool retValue = FACETRACKER_API_init_facetracker_resources(modelPath.c_str(),classiPath.c_str());
            FACETRACKER_API_setTrackingHidden();
            if(!retValue){
                LOGI("Init facetracker failed.....");
            }else{
                LOGI("Init facetracker success!!!");
            }
        }

        env->ReleaseStringUTFChars(mPath,dirPath);
    }
    m_glSample->InitGL();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xuwen_openglengine_render_CCOpenGLRender_ndkPaintGL(JNIEnv *env, jobject thiz) {
    m_glSample->PaintGL();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xuwen_openglengine_render_CCOpenGLRender_ndkResizeGL(JNIEnv *env, jobject thiz, jint width,
                                                       jint height) {
    m_glSample->ResizeGL(width,height);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xuwen_openglengine_render_CCOpenGLRender_ndkUpdateCameraFrame(JNIEnv *env, jobject obj,
                                                                jbyteArray yuvData,
                                                                jint width,
                                                                jint height) {
    //创建YUV数据buffer，类型为byte
    jbyte * yuvBuffer = (jbyte*)env->GetByteArrayElements(yuvData, 0);

    //设置OpenCV图片格式
    cv::Mat image(height + height/2,width,CV_8UC1,(unsigned char *)yuvBuffer);
    cv::Mat bgrCVFrame;//OpenCV通常用于数据处理的bgr图片数据格式
    cv::Mat yuvCVFrame;//yuv数据格式
    cv::cvtColor(image, bgrCVFrame, cv::COLOR_YUV2BGR_NV21);//将yuv数据格式转为bgr
    cv::rotate(bgrCVFrame, bgrCVFrame, cv::ROTATE_90_COUNTERCLOCKWISE);//将bgr数据旋转90度
    cv::flip(bgrCVFrame,bgrCVFrame,1);

    //对bgr帧数据进行人脸标定处理
    FACETRACKER_API_facetracker_obj_track(bgrCVFrame);
    //获取人脸标定中低29点位置，也就是人脸中心
    ofVec2f posVec2f = FACETRACKER_API_getPosition(bgrCVFrame);
//    LOGD("TRACKER: %f %f",posVec2f.x,posVec2f.y);
    //获取当前人脸移动导致的位置变换信息：以人脸中心点为基准
    glm::vec2 posValue = glm::vec2(posVec2f.x,posVec2f.y);
    //获取当前人脸移动导致的缩放信息
    float scaleValue = FACETRACKER_API_getScale(bgrCVFrame);
//    LOGD("TRACKER: %f %f",posVec2f.x,posVec2f.y);

    //将bgr数据存入yuvFrame中
    cv::cvtColor(bgrCVFrame, yuvCVFrame, cv::COLOR_BGR2YUV_I420);

    //OpenGL渲染YUV
    int lumaSize = width* height;//Y分量数据大小
    int uv_stride = width / 2;
    int chromaSize = uv_stride *(height / 2);//UV数据分量大小
    //定义YUV三个分量的起始数据地址
    uint8_t *Y_data_Dst = yuvCVFrame.data;
    uint8_t *U_data_Dst = yuvCVFrame.data + lumaSize;
    uint8_t *V_data_Dst = yuvCVFrame.data + lumaSize +chromaSize;

    //定义YUV变量
    YUVData_Frame yuvDataFrame;
    memset(&yuvDataFrame,0, sizeof(yuvDataFrame));
    //设置YUV每个分量的开始位置
    yuvDataFrame.luma.dataBuffer = Y_data_Dst;
    yuvDataFrame.chromaB.dataBuffer = U_data_Dst;
    yuvDataFrame.chromaR.dataBuffer = V_data_Dst;
    //设置YUV每个分量的大小
    yuvDataFrame.luma.length = lumaSize;
    yuvDataFrame.chromaB.length = chromaSize;
    yuvDataFrame.chromaR.length = chromaSize;
    //设置YUV的宽高
    yuvDataFrame.width = height;
    yuvDataFrame.height = width;

    //将YUV数据传入OpenGL
    m_glSample->RenderVideo(&yuvDataFrame);

    //传递当前人脸移动带来的位置变换及缩放信息
    m_glSample->UpdateTrackingInfo(posValue,scaleValue);

    //手动回到Java方法进行通知刷新画面
    if(obj != nullptr){
        jclass cvRenderClass = env->GetObjectClass(obj);
        jmethodID methodID = env->GetMethodID(cvRenderClass, "UpdateRequestGLRender", "()V");
        env->CallVoidMethod(obj,methodID);
        env->DeleteLocalRef(cvRenderClass);
    }

    env->ReleaseByteArrayElements(yuvData, yuvBuffer, 0);

}
extern "C"
JNIEXPORT void JNICALL
Java_com_xuwen_openglengine_render_CCOpenGLRender_ndkRecordVideo(JNIEnv *env, jobject thiz,
                                                          jboolean state) {
    if (state == JNI_TRUE){
        m_glSample->RecordVideo(true);
    }else{
        m_glSample->RecordVideo(false);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xuwen_openglengine_render_CCOpenGLRender_ndkSetRenderType(JNIEnv *env, jobject thiz,
                                                                   jint render_type) {
    m_sample_type = render_type;

    switch (render_type){
        case SAMPLE_TYPE_KEY_TEXTURE_2D:
            m_glSample = new CCSampleTexture2D();
            break;
        case SAMPLE_TYPE_KEY_3D_CAMERA:
            m_glSample = new CCGLScene();
            break;
        case SAMPLE_TYPE_TEXTURE_SCALE:
            m_glSample = new CCSampleTexture2DScale();
            break;
        case SAMPLE_TYPE_CAMERA:
            m_glSample = new CCGLCamera2D();
            break;
        case SAMPLE_TYPE_TRANSITION:
            m_glSample = new CCTranstionTexture();
            break;
        case SAMPLE_TYPE_GREENSCREEN:
            m_glSample = new CCGreenScreenTexture();
            break;
        case SAMPLE_TYPE_3D_MODEL:
            m_glSample = new Model3DSample();
            break;
        case SAMPLE_TYPE_3D_ANIM_MODEL:
            m_glSample = new Model3DAnimSample();
            break;
        case SAMPLE_TYPE_3D_ANIM_MODEL_1:
            m_glSample = new GL3DModel();
            break;
        default:
            break;
    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_xuwen_openglengine_render_CCOpenGLRender_ndkRelease(JNIEnv *env, jobject thiz) {
    m_glSample->Release();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xuwen_openglengine_render_CCOpenGLRender_ndkUpdateTransformMatrix(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jfloat rotate_x,
                                                                           jfloat rotate_y,
                                                                           jfloat scale_x,
                                                                           jfloat scale_y) {
    m_glSample->updateTransformMatrix(rotate_x,rotate_y,scale_x,scale_y);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xuwen_openglengine_render_CCOpenGLRender_ndkSetParamsInt(JNIEnv *env, jobject thiz,
                                                                  jint param_type, jint value0,
                                                                  jint value1) {
    // TODO: implement ndkSetParamsInt()
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xuwen_openglengine_render_CCOpenGLRender_ndkSetParamsFloat(JNIEnv *env, jobject thiz,
                                                                    jint param_type, jfloat value0,
                                                                    jfloat value1) {
    if(m_glSample){
        switch (param_type)
        {
            case SAMPLE_TYPE_KEY_SET_TOUCH_LOC:
                m_glSample->SetTouchLocation(value0, value1);
                break;
            case SAMPLE_TYPE_SET_GRAVITY_XY:
                m_glSample->SetGravityXY(value0, value1);
                break;
            default:
                break;

        }
    }
}