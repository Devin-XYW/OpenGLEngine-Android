#version 300 es
precision mediump float;

//转场动画对应的着色器地址：https://gl-transitions.com/editor/windowslice
//实现从左往右逐条变换的效果

uniform sampler2D texture_1;//当前纹理贴图1
uniform sampler2D texture_2;//当前纹理贴图2

in vec2 o_uv;//纹理坐标
out vec4 fragColor;//输出到下一阶段的片段颜色属性

uniform float u_offset;//一个0-1的uniform渐变量

const float count = 10.0;//控制从左向右，条状方块的数量
const float smoothness = 0.5;//控制每条方块的变换速度

vec4 transition(vec2 uv){
    float progress = u_offset;
    float pr = smoothstep(-smoothness,0.0,uv.x - progress *(1.0 + smoothness));
    //y = step(e,x): if x<e则y=0.0, if x>=e则y=1.0
    //fract(x),返回x-floor(x),即取小数部分
    float s = step(pr, fract(count * uv.x));
    return mix(texture(texture_1, uv), texture(texture_2, uv), s);
}

void main() {
    fragColor = transition(o_uv);
}
