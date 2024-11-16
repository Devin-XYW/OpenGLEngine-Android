#version 300 es
precision mediump float;

//实现简单的绿幕抠图，参考https://www.jianshu.com/p/159744875386
// 关键就是判断颜色范围，这里简单认定g>140.0 && r<128.0 && b<128.0 时为绿色。
// 当是绿色的时候，就将其颜色换成白色。同时alpha值设置为0.0

uniform sampler2D texture_1;//当前纹理贴图1
uniform sampler2D texture_2;//当前纹理贴图2

in vec2 o_uv;//纹理坐标
out vec4 fragColor;//输出到下一阶段的片段颜色属性

uniform float u_offset;//一个0-1的uniform渐变量

const float pixel = 30.0;

void main() {

    vec4 tc = texture(texture_1,o_uv);//对当前图片进行采样
    float r = tc.x * 255.0;
    float g = tc.y * 255.0;
    float b = tc.z * 255.0;

    // 这里简单的认定 g>140.0 && r<128.0 && b<128.0 时为绿色。
    // 当是绿色的时候，就将其颜色换成白色。同时alpha值设置为0.0
    if(g>140.0 && r<128.0 && b<128.0){
        tc.x =1.0;
        tc.y =1.0;
        tc.z =1.0;
        tc.w =0.0;
    }else{
        tc.w = 1.0;
    }

    // 这里，我们检测被采样的纹理颜色的alpha值是否低于0.1的阈值，
    //如果是的话，则丢弃这个片段。片段着色器保证了它只会渲染不是（几乎）完全透明的片段。
    if(tc.a < 0.1) discard;

    fragColor = tc;
}
