package com.xuwen.openglengine.render;

import static com.xuwen.openglengine.type.IMyNativeRendererType.SAMPLE_TYPE_SET_GRAVITY_XY;
import static com.xuwen.openglengine.type.IMyNativeRendererType.SAMPLE_TYPE_SET_TOUCH_LOC;

import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;

import com.xuwen.openglengine.activity.Camera3DActivity;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class CCOpenGLRender implements GLSurfaceView.Renderer {
    // Used to load the 'openglengine' library on application startup.
    static {
        System.loadLibrary("openglengine");
        System.loadLibrary("opencv_java4");
    }

    private final static String TAG = "CCOpenGLRender";

    private Context mContext;
    private String mDirectoryPath;
    public int mSampleType = 0;

    public CCOpenGLRender(Context context, String dirPathStr){
        mContext = context;
        mDirectoryPath = dirPathStr;
    }


    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        //获取AssetManager
        AssetManager assetManager = mContext.getAssets();
        //初始化OpenGL引擎
        ndkInitGL(assetManager,mDirectoryPath);
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        ndkResizeGL(width,height);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        ndkPaintGL();
    }

    /**
     * 通知更Camera采集的YUV数据
     * @param data 当前Camera采集的YUV数据
     * @param width YUV图片的width
     * @param height YUV图片的Height
     */
    public void UpdateCameraFrame(byte[] data,int width,int height){
        ndkUpdateCameraFrame(data,width,height);
    }

    /**
     * 回调activity中的方法进行手动刷新界面
     */
    public void UpdateRequestGLRender() {
        if(mContext != null){
            Camera3DActivity activity = (Camera3DActivity) mContext;
            if(activity != null){
                activity.RequestOpenGLRender();
            }
        }
    }

    public void setRender(int renderSampleType){
        mSampleType = renderSampleType;
        ndkSetRenderType(renderSampleType);
    }

    public void onDestroy(){
        ndkRelease();
    }

    public void RecordVideoWithStates(boolean bState){
        ndkRecordVideo(bState);
    }

    public void updateTransformMatrix(float rotateX,float rotateY,float scaleX,float scaleY){
        ndkUpdateTransformMatrix(rotateX,rotateY,scaleX,scaleY);
    }

    public void setTouchLoc(float x, float y)
    {
        ndkSetParamsFloat(SAMPLE_TYPE_SET_TOUCH_LOC, x, y);
    }

    public void setGravityXY(float x, float y) {
        ndkSetParamsFloat(SAMPLE_TYPE_SET_GRAVITY_XY, x, y);
    }


    //==================Native方法===================================
    //初始化当前Render
    private native void ndkSetRenderType(int renderType);
    //初始化GL
    private native void ndkInitGL(AssetManager assetManager,String path);
    //渲染画面
    private native void ndkPaintGL();
    //当前画面被Resize
    private native void ndkResizeGL(int width,int height);

    //传递当前用户在屏幕上进行缩放、旋转等信息
    private native void ndkUpdateTransformMatrix(float rotateX, float rotateY
            , float scaleX, float scaleY);

    //传递当前Camera采集的数据
    private native void ndkUpdateCameraFrame(byte[] yuvFrame,int width,int height);
    //通知当前render释放资源
    private native void ndkRelease();
    //录制视频
    private native void ndkRecordVideo(boolean state);

    //上层向引擎层传递int类型的数据
    private native void ndkSetParamsInt(int paramType, int value0, int value1);
    //上层向引擎层传递float类型的数据
    private native void ndkSetParamsFloat(int paramType, float value0, float value1);

}
