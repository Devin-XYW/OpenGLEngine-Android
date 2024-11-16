package com.xuwen.openglengine.activity

import android.Manifest
import android.app.Activity
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.util.SparseArray
import android.util.SparseIntArray
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.DividerItemDecoration
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.xuwen.openglengine.CommonUtils
import com.xuwen.openglengine.R
import com.xuwen.openglengine.adapter.GLRecyclerAdapter
import com.xuwen.openglengine.type.IMyNativeRendererType
import java.util.*

class MainActivity : AppCompatActivity() {

    companion object {
        const val ITEM_IMAGE = "item_image"
        const val ITEM_TITLE = "item_title"
        const val ITEM_SUBTITLE = "item_subtitle"
        val REQUEST_PERMISSIONS = arrayOf(
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE
        )
        const val PERMISSION_REQUEST_CODE = 1
    }

    val data: MutableList<Map<String, Any?>> = ArrayList()
    private val typeMapping = SparseIntArray()
    private val activityMapping = SparseArray<Class<out Activity?>>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            requestPermissions(
                MainActivity.REQUEST_PERMISSIONS,
                MainActivity.PERMISSION_REQUEST_CODE
            )
        }

        //加载main布局
        setContentView(R.layout.activity_main)

        //加载当前选择项
        initData()

        //加载recyclerView
        val recyclerView = findViewById<View>(R.id.recycler_view) as RecyclerView

        val layoutManager = LinearLayoutManager(this)
        //设置水平滚动
        layoutManager.orientation = LinearLayoutManager.VERTICAL
        recyclerView.layoutManager = layoutManager
        // 设置系统自带的间割线
        recyclerView.addItemDecoration(DividerItemDecoration(this, LinearLayoutManager.VERTICAL))

        val adapter = GLRecyclerAdapter(data, this)
        recyclerView.adapter = adapter

        // 设置点击事件回调接口
        adapter.setOnItemClickListener { position ->
            val type = typeMapping[position]
            Log.d("MainActivity", "type = $type")
            when (type) {
                IMyNativeRendererType.SAMPLE_TYPE_3D_CAMERA,
                IMyNativeRendererType.SAMPLE_TYPE_CAMERA -> {
                    // 取出要启动的Activity
                    val activityToLaunch = activityMapping[position]

                    Log.d("MainActivity", "activityToLaunch = $activityToLaunch")
                    if (activityToLaunch != null) {
                        val launchIntent = Intent(this@MainActivity, activityToLaunch)
                        launchIntent.putExtra(IMyNativeRendererType.RENDER_TYPE, type)
                        startActivity(launchIntent)
                    }
                }
                else -> {
                    val launchIntent =
                        Intent(this@MainActivity, NativeRenderActivity::class.java)
                    launchIntent.putExtra(IMyNativeRendererType.RENDER_TYPE, type)
                    startActivity(launchIntent)
                }
            }
        }
    }

    private fun initData(){
        var i=0

        val item0:MutableMap<String,Any?> = HashMap()
        item0[ITEM_IMAGE] = R.mipmap.ic_texture_map
        item0[ITEM_TITLE] = "展示一个基本的2D纹理渲染"
        item0[ITEM_SUBTITLE] = "基本2D纹理渲染"
        data.add(item0)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_2D_TEXTURE)

        val item1:MutableMap<String,Any?> = HashMap()
        item1[ITEM_IMAGE] = R.mipmap.ic_texture_map
        item1[ITEM_TITLE] = "基于OpenCV的3D特效引擎"
        item1[ITEM_SUBTITLE] = "使用OpenCV人脸标定并进行3D特效引擎渲染特效"
        data.add(item1)
        typeMapping.put(i, IMyNativeRendererType.SAMPLE_TYPE_3D_CAMERA)
        activityMapping.put(i, Camera3DActivity::class.java)
        i++

        val item2:MutableMap<String,Any?> = HashMap()
        item2[ITEM_IMAGE] = R.mipmap.ic_texture_map
        item2[ITEM_TITLE] = "对2D纹理渲染进行缩放、旋转等操作"
        item2[ITEM_SUBTITLE] = "对2D纹理渲染进行缩放、旋转等操作"
        data.add(item2)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_TEXTURE_SCALE)

        val item3:MutableMap<String,Any?> = HashMap()
        item3[ITEM_IMAGE] = R.mipmap.ic_texture_map
        item3[ITEM_TITLE] = "相机图像采集相关特效"
        item3[ITEM_SUBTITLE] = "对相机采样结果进行相关特效处理"
        data.add(item3)
        typeMapping.put(i, IMyNativeRendererType.SAMPLE_TYPE_CAMERA)
        activityMapping.put(i, Camera3DActivity::class.java)
        i++

        val item4:MutableMap<String,Any?> = HashMap()
        item4[ITEM_IMAGE] = R.mipmap.ic_texture_map
        item4[ITEM_TITLE] = "转场切换动画"
        item4[ITEM_SUBTITLE] = "对2D纹理进行转场动画处理"
        data.add(item4)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_TRANSITION)

        val item5:MutableMap<String,Any?> = HashMap()
        item5[ITEM_IMAGE] = R.mipmap.ic_texture_map
        item5[ITEM_TITLE] = "实现绿幕抠图"
        item5[ITEM_SUBTITLE] = "对2D纹理进行绿幕抠图操作"
        data.add(item5)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_GREENSCREEN)

        val item6:MutableMap<String,Any?> = HashMap()
        item6[ITEM_IMAGE] = R.mipmap.ic_texture_map
        item6[ITEM_TITLE] = "渲染一个3D模型"
        item6[ITEM_SUBTITLE] = "渲染一个3D模型，可进行拖动操作"
        data.add(item6)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_3D_MODEL)

        val item7:MutableMap<String,Any?> = HashMap()
        item7[ITEM_IMAGE] = R.mipmap.ic_texture_map
        item7[ITEM_TITLE] = "渲染一个3D动画模型"
        item7[ITEM_SUBTITLE] = "渲染一个3D动画模型，带动画，并可进行拖动操作"
        data.add(item7)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_3D_ANIM_MODEL)

        val item8:MutableMap<String,Any?> = HashMap()
        item8[ITEM_IMAGE] = R.mipmap.ic_texture_map
        item8[ITEM_TITLE] = "渲染一个3D动画模型"
        item8[ITEM_SUBTITLE] = "渲染一个3D动画模型，带动画，并可进行拖动操作"
        data.add(item8)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_3D_ANIM_MODEL_1)
    }

    protected fun hasPermissionsGranted(permissions: Array<String>): Boolean {
        for (permission in permissions) {
            if (checkSelfPermission(permission!!)
                != PackageManager.PERMISSION_GRANTED
            ) {
                return false
            }
        }
        return true
    }


}



