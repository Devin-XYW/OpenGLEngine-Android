package com.xuwen.openglengine.activity

import android.Manifest
import android.app.Activity
import android.content.pm.PackageManager
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.os.Environment
import android.view.ViewGroup
import android.widget.RelativeLayout
import android.widget.Toast
import androidx.core.app.ActivityCompat
import com.xuwen.openglengine.CommonUtils
import com.xuwen.openglengine.R
import com.xuwen.openglengine.render.CCOpenGLRender
import com.xuwen.openglengine.surfaceview.MyCustomerGLSurfaceView
import com.xuwen.openglengine.surfaceview.MyGLSurfaceView
import com.xuwen.openglengine.type.IMyNativeRendererType
import java.io.File

class NativeRenderActivity:Activity(){

    private var mMinSetting = -1
    private var mMagSetting = -1

    private var mDirPath: String? = null

    companion object {
        private const val CONTEXT_CLIENT_VERSION = 3
        private const val MIN_SETTING = "min_setting"
        private const val MAG_SETTING = "mag_setting"
        private const val TAG: String = "NativeRenderActivity"

        private val REQUEST_PERMISSIONS = arrayOf(
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.RECORD_AUDIO
        )
        private const val PERMISSION_REQUEST_CODE = 1
    }

    private var mRootView: ViewGroup? = null
    private var mUpdateGLSurfaceView: MyGLSurfaceView? = null
    private var mStaicGLSurfaceView: MyCustomerGLSurfaceView? = null
    private var renderer: CCOpenGLRender? = null
    var type = IMyNativeRendererType.SAMPLE_TYPE

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        //定义接收数据的Intent
        val intent = intent
        type = intent.getIntExtra(
            IMyNativeRendererType.RENDER_TYPE,
            IMyNativeRendererType.SAMPLE_TYPE
        )

        //加载Render
        renderer = CCOpenGLRender(this,"")
        renderer?.let { myNativeRender ->
            myNativeRender.setRender(type)

            setContentView(R.layout.activity_native_render)
            mRootView = findViewById(R.id.rootView) as ViewGroup

            if(type == IMyNativeRendererType.SAMPLE_TYPE_3D_MODEL ||
                    type == IMyNativeRendererType.SAMPLE_TYPE_3D_ANIM_MODEL ||
                    type == IMyNativeRendererType.SAMPLE_TYPE_3D_ANIM_MODEL_1){
                mUpdateGLSurfaceView = MyGLSurfaceView(this,myNativeRender)
                mUpdateGLSurfaceView!!.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                val lp = RelativeLayout.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT
                )
                lp.addRule(RelativeLayout.CENTER_IN_PARENT)
                mRootView!!.addView(mUpdateGLSurfaceView, lp)

                if (mRootView!!.width != mUpdateGLSurfaceView!!.width || mRootView!!.height != mUpdateGLSurfaceView!!.height) {
                    mUpdateGLSurfaceView!!.setAspectRatio(mRootView!!.width, mRootView!!.height)
                }

            }else{
                mStaicGLSurfaceView = MyCustomerGLSurfaceView(this,myNativeRender, CONTEXT_CLIENT_VERSION)
                val lp = RelativeLayout.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT
                )
                lp.addRule(RelativeLayout.CENTER_IN_PARENT)
                mRootView!!.addView(mStaicGLSurfaceView, lp)

                if (mRootView!!.width != mStaicGLSurfaceView!!.width || mRootView!!.height != mStaicGLSurfaceView!!.height) {
                    mStaicGLSurfaceView!!.setAspectRatio(mRootView!!.width, mRootView!!.height)
                }
            }
        }
    }

    override fun onResume() {
        super.onResume()
        if(type == IMyNativeRendererType.SAMPLE_TYPE_3D_MODEL
            || type == IMyNativeRendererType.SAMPLE_TYPE_3D_ANIM_MODEL ||
            type == IMyNativeRendererType.SAMPLE_TYPE_3D_ANIM_MODEL_1){
            mUpdateGLSurfaceView?.onResume()
        }else{
            mStaicGLSurfaceView?.onResume()
        }
    }

    private fun hasPermissionsGranted(permissions: Array<String>): Boolean {
        for (permission in permissions) {
            if (ActivityCompat.checkSelfPermission(this, permission)
                != PackageManager.PERMISSION_GRANTED) {
                return false
            }
        }
        return true
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String?>, grantResults: IntArray) {
        if (requestCode == PERMISSION_REQUEST_CODE) {
            if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
                Toast.makeText(this, "We need the permission: RECORD_AUDIO && WRITE_EXTERNAL_STORAGE", Toast.LENGTH_SHORT).show()
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        }
    }

    override fun onPause() {
        super.onPause()
        if(type == IMyNativeRendererType.SAMPLE_TYPE_3D_MODEL ||
            type == IMyNativeRendererType.SAMPLE_TYPE_3D_ANIM_MODEL ||
            type == IMyNativeRendererType.SAMPLE_TYPE_3D_ANIM_MODEL_1){
            mUpdateGLSurfaceView?.onPause()
        }else{
            mStaicGLSurfaceView?.onPause()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        renderer?.onDestroy()
    }

    /**
     * GLSurfaceView会在一个单独的线程中调用渲染器的方法。
     * 默认情况下，GLSurfaceView 会以显示设备的刷新频率不断地渲染，当然，它也可以配置为按请求渲染，
     * 只需要用 GLSurfaceView.RENDERMODE_WHEN_DIRTY作为参数调用GLSurfaceView.setRenderMode() 即可。
     *  GLSurfaceView.RENDERMODE_WHEN_DIRTY
     *  GLSurfaceView.RENDERMODE_CONTINUOUSLY
     */
    private fun setRenderMode(it: MyCustomerGLSurfaceView) {
        // 默认渲染模式设置为RENDERMODE_WHEN_DIRTY
        it.renderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY
    }

    override fun onSaveInstanceState(outState: Bundle) {
        outState.putInt(MIN_SETTING, mMinSetting)
        outState.putInt(MAG_SETTING, mMagSetting)
    }

    private var m_3DFile: File? = null
    private var m_3DFile_t: File? = null
    private var m_3DFile_d: File? = null
}