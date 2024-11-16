package com.xuwen.openglengine.camera;

import android.app.Activity;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.Surface;

import com.xuwen.openglengine.render.CCOpenGLRender;

import java.io.IOException;

public class CCOpenGLCamera implements Camera.PreviewCallback{

    private final String TAG = "CCOpenGLCamera";
    private Camera mCamera;//当前Camera类
    private int mCameraId;//当前CameraId

    private Camera.Size mVideoSize;//当前视频大小
    private CCOpenGLRender mRender;//当前Render类

    private int[] mCameraTexture = new int[1];
    private SurfaceTexture mTexture;

    private GLSurfaceView mGLView;//当前预览的SurfaceView

    public CCOpenGLCamera(CCOpenGLRender render,GLSurfaceView glView){

        //初始化当前Render和SurfaceView
        mRender = render;
        mGLView = glView;

        //设置相机为前置摄像头
        mCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;

        createOesTexture();
        mTexture = new SurfaceTexture(mCameraTexture[0]);

    }

    /**
     * 对外提供初始化相机
     */
    public void InitCameraPermissionGranted(){

        mCamera = openCamera();

        try {
            //设置当前相机的旋转角度
            updateDisplayOrientation(mCameraId,mCamera);
            Camera.Parameters parameters = mCamera.getParameters();
            //设置当前相机采集的图片格式
            parameters.setPreviewFormat(ImageFormat.NV21);
            mCamera.setParameters(parameters);
            mCamera.setPreviewTexture(mTexture);
            mCamera.setPreviewCallback(this);
            mCamera.startPreview();
        }catch (IOException e){

        }
    }

    /**
     * 初始化并打开相机
     * @return 返回相机实例
     */
    private Camera openCamera(){
        Camera camera;
        try {
            camera = Camera.open(mCameraId);
        }catch (Exception e){
            camera = null;
        }
        return camera;
    }

    /**
     * 设置当前Camera的显示，方式为贴纸
     */
    private void createOesTexture(){
        GLES30.glGenTextures(1,mCameraTexture,0);
    }

    @Override
    public void onPreviewFrame(byte[] bytes, Camera camera) {
        //采集到图片YUV数据回调
        this.mVideoSize = camera.getParameters().getPreviewSize();

        if(mRender != null){
            mRender.UpdateCameraFrame(bytes,mVideoSize.width,mVideoSize.height);
        }
    }

    /**
     * 设置相机和图片方向
     */
    private void updateDisplayOrientation(int id,Camera camera) {
        if (mCamera == null) return;

        Activity targetActivity = (Activity) mGLView.getContext();
        //获取当前相机信息
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(id, info);

        //获取当前手机屏幕方向
        int rotation = targetActivity.getWindowManager().getDefaultDisplay().getRotation();
        int degrees = 0;//记录当前屏幕方向对应的角度
        switch (rotation) {
            case Surface.ROTATION_0:
                degrees = 0;
                break;
            case Surface.ROTATION_90:
                degrees = 90;
                break;
            case Surface.ROTATION_180:
                degrees = 180;
                break;
            case Surface.ROTATION_270:
                degrees = 270;
                break;
        }
        int displayDegree;//计算当前需要进行旋转的角度
        if(info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT){
            //如果当前是前置摄像头
            displayDegree = (info.orientation + degrees) % 360;
            displayDegree = (360 - displayDegree) % 360;
        }else {
            displayDegree = (info.orientation - degrees + 360) % 360;
        }

        Log.d(TAG,"SET ORIENTATION...:"+displayDegree);
        camera.setDisplayOrientation(displayDegree);
    }

    public void DestoryCamera(){
        if(mCamera != null){
            mCamera.setPreviewCallback(null);
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }
    }
}
