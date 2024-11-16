#version 300 es
precision mediump float;

uniform sampler2D texture_1;//当前纹理贴图1
uniform sampler2D texture_2;//当前纹理贴图2

in vec2 o_uv;//纹理坐标
out vec4 fragColor;//输出到下一阶段的片段颜色属性

uniform float u_offset;//一个0-1的uniform渐变量

//使用渐变量对两张纹理进行mix混合处理
vec4 transition(vec2 uv){
    vec4 result = mix(texture(texture_1,uv),texture(texture_2,uv),u_offset);
    return result;
}

void main() {
    fragColor = transition(o_uv);
}