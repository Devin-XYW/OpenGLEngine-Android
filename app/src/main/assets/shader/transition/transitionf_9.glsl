#version 300 es
precision mediump float;

//转场动画对应的着色器地址：https://gl-transitions.com/editor/WaterDrop

uniform sampler2D texture_1;//当前纹理贴图1
uniform sampler2D texture_2;//当前纹理贴图2

in vec2 o_uv;//纹理坐标
out vec4 fragColor;//输出到下一阶段的片段颜色属性

uniform float u_offset;//一个0-1的uniform渐变量
uniform vec2 textSize;//纹理尺寸

const float amplitude = 30.0;
const float speed = 30.0;

vec4 transition(vec2 uv){
    vec2 dir = uv - vec2(0.5);
    float dist = length(dir);//距离圆心的距离

    if(dist > u_offset){
        vec4 result = mix(
            texture(texture_1,uv),
            texture(texture_2,uv),
            u_offset
        );
        return result;
    }else{
        vec2 offset = dir * sin(dist * amplitude - u_offset * speed);
        vec4 result = mix(
            texture(texture_1,uv + offset),
            texture(texture_2,uv),
            u_offset
        );
        return result;
    }
}

void main() {
    fragColor = transition(o_uv);
}
