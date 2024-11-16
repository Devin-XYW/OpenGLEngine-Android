package com.xuwen.openglengine.activity;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import com.xuwen.openglengine.camera.CCOpenGLCamera;
import com.xuwen.openglengine.R;
import com.xuwen.openglengine.render.CCOpenGLRender;
import com.xuwen.openglengine.type.IMyNativeRendererType;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class Camera3DActivity extends AppCompatActivity {

    private final static String TAG = "MainActivity";

    private  final int REQUEST_EXTERNAL_STORAGE = 1;
    private  final int REQUEST_CAMERA = 2;

    //动态申请的权限
    private  String[] PERMISSIONS_STORAGE = {Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE };
    private  String[] PERMISSIONS_CAMERA = new String[]{Manifest.permission.CAMERA};

    private Button  m_recordButton;

    private CCOpenGLCamera mCamera;
    private GLSurfaceView mGLSurFaceView;
    private CCOpenGLRender mRender;

    //当前OpenCV人脸标定模型在手机本地存储路径
    private String  mDirPath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent intent = getIntent();
        int type = intent.getIntExtra(
                IMyNativeRendererType.RENDER_TYPE,
                IMyNativeRendererType.SAMPLE_TYPE);

        //设置屏幕方向为竖屏
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        requestWindowFeature(Window.FEATURE_NO_TITLE);//设置隐藏标题栏
        //设置隐藏状态栏
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.camera_activity);

        loadFaceTrackerModelFiles();
        load3DModelFiles();
        getFilePrivateDirectoryPath();

        //初始化Render
        mRender = new CCOpenGLRender(this,mDirPath);
        mRender.setRender(type);
        //初始化SurfaceView
        mGLSurFaceView = findViewById(R.id.glSurfaceView);
        mGLSurFaceView.setEGLContextClientVersion(3);//设置支持的OpenGL版本
        mGLSurFaceView.setRenderer(mRender);
        //设置当前SurfaceView刷新方式为手动刷新
        mGLSurFaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        mGLSurFaceView.bringToFront();

        //创建摄像头
        mCamera = new CCOpenGLCamera(mRender,mGLSurFaceView);

        verifyStoragePermission(this);

        m_recordButton = (Button) findViewById(R.id.recordButton);
        m_recordButton.setSelected(true);
        m_recordButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean bSelected = m_recordButton.isSelected();
                if(bSelected){
                    m_recordButton.setBackgroundResource(R.mipmap.stop_bg);
                }else{
                    m_recordButton.setBackgroundResource(R.mipmap.record_bg);
                    Toast.makeText(getApplicationContext(),R.string.toast,Toast.LENGTH_SHORT).show();
                }
                mRender.RecordVideoWithStates(bSelected);
                m_recordButton.setSelected(!bSelected);

            }
        });

    }

    /**
     * 动态加载权限
     */
    private void verifyStoragePermission(Activity activity){

        //动态申请权限
        int cameraPermission = ActivityCompat
                .checkSelfPermission(activity,Manifest.permission.CAMERA);

        //判断当前用户选择的相机权限状态
        if (cameraPermission != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(activity, PERMISSIONS_CAMERA, REQUEST_CAMERA);
        }else{
            //初始化相机
            mCamera.InitCameraPermissionGranted();
        }

        int permission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.WRITE_EXTERNAL_STORAGE);
        if (permission != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(activity, PERMISSIONS_STORAGE, REQUEST_EXTERNAL_STORAGE);
        }
    }

    //当用户选择相机权限后会回调这个方法
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (requestCode == REQUEST_CAMERA && grantResults.length > 0
                && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            mCamera.InitCameraPermissionGranted();
        }
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        //释放Camera状态
        mCamera.DestoryCamera();
        mRender.onDestroy();
    }

    /**
     * 手动回调进行刷新Render
     */
    public void RequestOpenGLRender(){
        mGLSurFaceView.requestRender();
    }


    private File mClassifierFile;
    private File mModelFile;
    /**
     * 将assets目录下的模型文件拷贝到手机中应用私有目录下存储
     */
    private void loadFaceTrackerModelFiles(){

        try{
            //在应用私有目录下创建一个model目录：/data/user/0/com.xuwen.openglengine/model
            File modelDir = getDir("model", Context.MODE_PRIVATE);
            byte[] buffer = new byte[4096];
            int bytesRead;

            //将OpenCV人脸标定模型文件haar_roboman_ff_alt2.xml拷贝到手机应用目录
            InputStream inputStream = getAssets().open("model/haar_roboman_ff_alt2.xml");
            mClassifierFile = new File(modelDir, "haar_roboman_ff_alt2.xml");
            FileOutputStream outputStream = new FileOutputStream(mClassifierFile);
            while((bytesRead = inputStream.read(buffer)) != -1){
                outputStream.write(buffer,0,bytesRead);
            }
            inputStream.close();
            outputStream.close();

            //将人脸标定模型文件roboman-landmark-model.bin拷贝到手机应用目录
            InputStream inputStream_r = getAssets().open("model/roboman-landmark-model.bin");
            mModelFile = new File(modelDir, "roboman-landmark-model.bin");
            FileOutputStream outputStream_r = new FileOutputStream(mModelFile);
            while ((bytesRead = inputStream_r.read(buffer)) != -1){
                outputStream_r.write(buffer,0,bytesRead);
            }
            inputStream_r.close();
            outputStream_r.close();

            modelDir.delete();

        }catch (Exception e){
            e.printStackTrace();
            Log.d("xuwen","Failed to load facetracker model. Exception thrown: " + e);
        }
    }

    private File m_3DFile;
    private File m_3DFile_t;
    private File m_3DFile_d;
    //读取OBJ文件到私有文件目录
    private void load3DModelFiles(){

        try{
            File modelDir = getDir("model",Context.MODE_PRIVATE);
            byte[] buffer = new byte[4096];
            int bytesRead;
            //读取obj文件
            InputStream inStream_c = getAssets().open("obj/monkey.obj");
            m_3DFile = new File(modelDir, "monkey.obj");
            FileOutputStream outStream_c = new FileOutputStream(m_3DFile);
            while ((bytesRead = inStream_c.read(buffer)) != -1) {
                outStream_c.write(buffer, 0, bytesRead);
            }
            inStream_c.close();
            outStream_c.close();

            //读取obj文件
            InputStream inStream_t = getAssets().open("obj/tortoise.obj");
            m_3DFile_t = new File(modelDir, "tortoise.obj");
            FileOutputStream outStream_t = new FileOutputStream(m_3DFile_t);
            while ((bytesRead = inStream_t.read(buffer)) != -1) {
                outStream_t.write(buffer, 0, bytesRead);
            }
            inStream_t.close();
            outStream_t.close();

            //读取obj文件
            InputStream inStream_d = getAssets().open("obj/dragon.obj");
            m_3DFile_d = new File(modelDir, "dragon.obj");
            FileOutputStream outStream_d = new FileOutputStream(m_3DFile_d);
            while ((bytesRead = inStream_d.read(buffer)) != -1) {
                outStream_d.write(buffer, 0, bytesRead);
            }
            inStream_d.close();
            outStream_d.close();

            modelDir.delete();
        } catch (IOException e){
            e.printStackTrace();
            //Log.d("","Failed to load facetracker model. Exception thrown: " + e);
        }
    }

    /**
     * 获取当前手机中存放模型文件的绝对路径
     */
    private void getFilePrivateDirectoryPath(){
        File modelDir = getDir("model",Context.MODE_PRIVATE);
        File mEmptyFile = new File(modelDir,"");
        mDirPath = mEmptyFile.getAbsolutePath();
        modelDir.delete();
        Log.d("OpenCV","opencv model file Path: " + mDirPath);
    }


}