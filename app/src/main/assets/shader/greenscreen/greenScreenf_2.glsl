#version 300 es
precision mediump float;

//绿幕抠图实现2：首先将rgb转换为hsv，通过rgb的g和hsv的颜色范围来确定绿色
//参考 https://blog.csdn.net/u010302327/article/details/109842237

uniform sampler2D texture_1;//当前纹理贴图1
uniform sampler2D texture_2;//当前纹理贴图2

in vec2 o_uv;//纹理坐标
out vec4 fragColor;//输出到下一阶段的片段颜色属性

uniform float u_offset;//一个0-1的uniform渐变量

//根据图片需求进行调节此值，太小会扣太多，太大会太少
float smoothValue = 0.05;

//首先将rgb转换为hsv,通过rgb和hsv的颜色范围来确定绿色，再使用算法
highp vec3 rgbTohsv(highp vec3 c){
    highp vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    highp vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    highp vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    highp float d = q.x - min(q.w, q.y);
    highp float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}


void main() {

    //抠图背景图
    highp vec4 backGroundRgba = texture(texture_2,o_uv);

    //对绿幕图进行抠图
    highp vec4 rgba = texture(texture_1,o_uv);
    highp float rbAverage = (rgba.r + rgba.b) * 0.8;
    //首先将rgb转换为hsv，通过rgb的g和hsv的颜色范围来确定绿色，再使用算法
    highp vec3 hsv = rgbTohsv(rgba.rgb);

    highp float hmin = 0.19444000;
    highp float hmax = 0.42777888;
    highp float smin = 0.16862000;
    highp float smax = 1.0;
    highp float vmin = 0.18039000;
    highp float vmax = 1.0;
    int gs = 0;
    //确定绿色范围
    if (hsv.x >= hmin && hsv.x <= hmax &&
    hsv.y >= smin && hsv.y <= smax &&
    hsv.z >= vmin && hsv.z <= vmax){
        gs = 1;
    } else if (rgba.g >= rbAverage && rgba.g > 0.6){
        gs = 1;
    }

    //对绿色范围的图像进行处理
    if(gs == 1){
        rbAverage = (rgba.r + rgba.b)*0.65;
        if (rbAverage > rgba.g)rbAverage = rgba.g;
        highp float gDelta = rgba.g - rbAverage;
        highp float ss = smoothstep(0.0, smoothValue, gDelta);
        rgba.a = 1.0 - ss;
        rgba.a = rgba.a * rgba.a * rgba.a;
        rgba = mix(vec4(0.0), rgba, rgba.a);
    }

//    // 这里，我们检测被采样的纹理颜色的alpha值是否低于0.1的阈值，
//    //如果是的话，则丢弃这个片段。片段着色器保证了它只会渲染不是（几乎）完全透明的片段。
//    if (rgba.a<0.1) discard;

    // 如果rgba.a < 0.1 说明是被替换掉的绿幕，这时候把两个纹理添加到一起
    if (rgba.a < 0.1) {
        fragColor.r = backGroundRgba.a * backGroundRgba.r + rgba.r * (1.0 - backGroundRgba.a);
        fragColor.g = backGroundRgba.a * backGroundRgba.g + rgba.g * (1.0 - backGroundRgba.a);
        fragColor.b = backGroundRgba.a * backGroundRgba.b + rgba.b * (1.0 - backGroundRgba.a);
        fragColor.a = backGroundRgba.a;
    } else {
        // 从绿幕背景中抠出来的图像
        fragColor = rgba;
    }

}
