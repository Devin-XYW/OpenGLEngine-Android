package com.xuwen.openglengine;

import android.app.Application;

public class IAApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        //加载模型到对应目录
        loadModel();
    }

    private void loadModel(){
        String fileDir = getCacheDir().getAbsolutePath();
        CommonUtils.copyAssetsDirToSDCard(this, "obj/avata1", fileDir+"/model");
        CommonUtils.copyAssetsDirToSDCard(this, "obj/vampire", fileDir+"/model");
        CommonUtils.copyAssetsDirToSDCard(this, "obj/3Dmodel", fileDir+"/model");
//        CommonUtils.copyAssetsDirToSDCard(this, "obj/AUDS", fileDir+"/model");
    }
}
