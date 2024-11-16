package com.xuwen.openglengine.surfaceview

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.util.Log
import android.view.MotionEvent
import android.view.ScaleGestureDetector
import com.xuwen.openglengine.render.CCOpenGLRender
import com.xuwen.openglengine.type.IMyNativeRendererType

/**
 * Surface
 */
class MyCustomerGLSurfaceView: GLSurfaceView,ScaleGestureDetector.OnScaleGestureListener{

    private lateinit var mRenderer: CCOpenGLRender
    private lateinit var mScaleGestureDetector: ScaleGestureDetector

    private var mDensity = 0f

    private var mPreviousX = 0f
    private var mPreviousY = 0f

    private var mXAngle = 0f
    private var mYAngle = 0f

    private var mRatioWidth = 0
    private var mRatioHeight = 0

    private var mPreScale = 1.0f
    private var mCurScale = 1.0f

    private var mLastMultiTouchTime: Long = 0

    constructor(context: Context?) : super(context) {}

    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs) {}

    constructor(context: Context?, glRender: CCOpenGLRender, eglContextVersion: Int) : this(context, null, glRender, eglContextVersion)

    constructor(context: Context?, attrs: AttributeSet?, glRender: CCOpenGLRender, eglContextVersion: Int) : super(context, attrs) {
        setEGLContextClientVersion(eglContextVersion)
        mRenderer = glRender

        /*If no setEGLConfigChooser method is called,
        then by default the view will choose an RGB_888 surface with a depth buffer depth of at least 16 bits.*/
        // 最后 2 个参数表示分别配置 16 位的深度缓冲区和模板缓冲区
        setEGLConfigChooser(8, 8, 8, 8, 16, 8)
        setRenderer(mRenderer)
        mScaleGestureDetector = ScaleGestureDetector(context, this)
    }

    // Hides superclass method.
    fun setRenderer(renderer: Renderer, density: Float) {
        mRenderer = renderer as CCOpenGLRender
        mDensity = density
        super.setRenderer(renderer)
    }

    companion object {
        private const val TOUCH_SCALE_FACTOR = 1.0f / 1000f
        private const val TAG = "MyCustomerGLSurfaceView"
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        Log.d(TAG, "onTouchEvent")
        if (event.pointerCount == 1) {
            Log.d(TAG, "event.pointerCount == 1")
            var x: Float = -1.0f
            var y: Float = -1.0f
            when (event.action) {
                MotionEvent.ACTION_DOWN -> {
                    mPreviousX = event.x
                    mPreviousY = event.y
                    mYAngle = 0f
                    mXAngle = 0f
                }
                MotionEvent.ACTION_MOVE -> {
                    val dy = event.y - mPreviousY
                    val dx = event.x - mPreviousX
                    mYAngle += (dy * TOUCH_SCALE_FACTOR).toInt()
                    mXAngle += (dx * TOUCH_SCALE_FACTOR).toInt()
                }
            }

            when (mRenderer.mSampleType) {
                IMyNativeRendererType.SAMPLE_TYPE_TEXTURE_SCALE -> {
                    Log.d(TAG, "updateTransformMatrix,mXAngle="+mXAngle+",mYAngle="+mYAngle)
                    mRenderer.updateTransformMatrix(-mXAngle, -mYAngle, mCurScale, mCurScale)
                    requestRender()
                }
            }
        } else {
            Log.d(TAG, "event.pointerCount != 1")
            mScaleGestureDetector.onTouchEvent(event)
        }
        return true
    }

    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec)
        val width = MeasureSpec.getSize(widthMeasureSpec)
        val height = MeasureSpec.getSize(heightMeasureSpec)
        if (0 == mRatioWidth || 0 == mRatioHeight) {
            setMeasuredDimension(width, height)
        } else {
            if (width < height * mRatioWidth / mRatioHeight) {
                setMeasuredDimension(width, width * mRatioHeight / mRatioWidth)
            } else {
                setMeasuredDimension(height * mRatioWidth / mRatioHeight, height)
            }
        }
    }

    fun setAspectRatio(width: Int, height: Int) {
        Log.d(TAG, "setAspectRatio() called with: width = [$width], height = [$height]")
        require(!(width < 0 || height < 0)) { "Size cannot be negative." }
        mRatioWidth = width
        mRatioHeight = height
        requestLayout()
    }


    override fun onScale(detector: ScaleGestureDetector?): Boolean {
        when (mRenderer.mSampleType) {
            IMyNativeRendererType.SAMPLE_TYPE_TEXTURE_SCALE -> {
                val preSpan = detector!!.previousSpan
                val curSpan = detector.currentSpan
                mCurScale = if (curSpan < preSpan) {
                    mPreScale - (preSpan - curSpan) / 200
                } else {
                    mPreScale + (curSpan - preSpan) / 200
                }
                mCurScale = 0.05f.coerceAtLeast(mCurScale.coerceAtMost(80.0f))
                mRenderer.updateTransformMatrix(mXAngle, mYAngle, mCurScale, mCurScale)
                requestRender()
            }
            else -> {}
        }
        return false
    }

    override fun onScaleBegin(p0: ScaleGestureDetector?): Boolean {
        return true
    }

    override fun onScaleEnd(p0: ScaleGestureDetector?) {
        mPreScale = mCurScale
        mLastMultiTouchTime = System.currentTimeMillis()
    }
}