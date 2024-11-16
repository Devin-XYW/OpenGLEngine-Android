#version 300 es
precision mediump float;

//转场动画对应的着色器地址：https://gl-transitions.com/editor/circleopen

uniform sampler2D texture_1;//当前纹理贴图1
uniform sampler2D texture_2;//当前纹理贴图2

in vec2 o_uv;//纹理坐标
out vec4 fragColor;//输出到下一阶段的片段颜色属性

uniform float u_offset;//一个0-1的uniform渐变量
uniform vec2 textSize;//纹理尺寸

const float smoothness = 0.3;
const bool opening = true;//标定圆形效果从中心向外扩散
const vec2 center = vec2(0.5,0.5);//圆心
const float SQRT_2 = 1.414213562373;

vec4 transition(vec2 uv){
    float x = opening ? u_offset : 1.0 - u_offset;
    float m = smoothstep(-smoothness,0.0
                    ,SQRT_2*distance(center,uv) - x*(1.0+smoothness));
    vec4 result = mix(
        texture(texture_1,uv),
        texture(texture_2,uv),
        opening ? 1.0-m : m
    );
    return result;
}

void main() {
    fragColor = transition(o_uv);
}
