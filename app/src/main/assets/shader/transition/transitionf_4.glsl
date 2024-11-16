#version 300 es
precision mediump float;

//转场动画对应的着色器地址：https://gl-transitions.com/editor/PolkaDotsCurtain

uniform sampler2D texture_1;//当前纹理贴图1
uniform sampler2D texture_2;//当前纹理贴图2

in vec2 o_uv;//纹理坐标
out vec4 fragColor;//输出到下一阶段的片段颜色属性

uniform float u_offset;//一个0-1的uniform渐变量

const float SQRT_2 = 1.414213562373;
const float dots  = 20.0;
const vec2 center = vec2(0.5, 0.5);

vec4 transition(vec2 uv){
    bool nextImage = distance(fract(uv * dots),vec2(0.5,0.5))
                        < (u_offset / distance(uv,center));
    return nextImage ? texture(texture_1,uv) : texture(texture_2,uv);
}

void main() {
    fragColor = transition(o_uv);
}
