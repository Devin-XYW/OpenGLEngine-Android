#version 300 es
precision mediump float;

//转场动画对应的着色器地址：https://gl-transitions.com/editor/CircleCrop

uniform sampler2D texture_1;//当前纹理贴图1
uniform sampler2D texture_2;//当前纹理贴图2

in vec2 o_uv;//纹理坐标
out vec4 fragColor;//输出到下一阶段的片段颜色属性

uniform float u_offset;//一个0-1的uniform渐变量
uniform vec2 textSize;//纹理尺寸

const float smoothnessValue = 1.0;
const float PI = 3.141592653589;

vec4 transition(vec2 p){
    vec2 rp = p*2.0 - 1.0;
    //atan()：三角函数中的tan值
    //smoothstep (float edge0,float edge1, float x):
        //Returns 0.0 if x <= edge0
        //1.0 if x>= edge1
        //performs smooth Hermite interpolation between 0.0 and 1.0 when edge0 < x < edge1.
    vec4 result = mix(
        texture(texture_2,p),
        texture(texture_1,p),
        smoothstep(0.0,smoothnessValue,atan(rp.y,rp.x) - (u_offset -0.5) * PI * 2.5)
    );
    return result;
}

void main() {
    fragColor = transition(o_uv);
}
