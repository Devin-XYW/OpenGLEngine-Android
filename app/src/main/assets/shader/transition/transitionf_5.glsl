#version 300 es
precision mediump float;

//转场动画对应的着色器地址：https://gl-transitions.com/editor/CircleCrop

uniform sampler2D texture_1;//当前纹理贴图1
uniform sampler2D texture_2;//当前纹理贴图2

in vec2 o_uv;//纹理坐标
out vec4 fragColor;//输出到下一阶段的片段颜色属性

uniform float u_offset;//一个0-1的uniform渐变量
uniform vec2 textSize;//纹理尺寸

const vec4 bgcolor = vec4(0.0,0.0,0.0,1.0);//转换过程中的背景色

vec4 transition(vec2 uv){

    float ratio = textSize.y / textSize.x;//宽高比例，通常为屏幕宽度
    vec2 ratio2 = vec2(1.0,1.0/ratio);//计算圆的半径时，需要计算出宽高比例

    //计算到圆心(0.5,0.5)的距离，同时需要乘宽高比例
    float dist = length((vec2(uv) - 0.5) * ratio2);

    float s = pow(2.0 * abs(u_offset - 0.5),3.0);

    //step (float edge, float x): Returns 0.0 if x < edge; otherwise, it returns 1.0.
    vec4 result = mix(
        u_offset < 0.5 ? texture(texture_1,uv) : texture(texture_2,uv),
        bgcolor,
        step(s,dist)
    );
    return result;
}

void main() {
    fragColor = transition(o_uv);
}
