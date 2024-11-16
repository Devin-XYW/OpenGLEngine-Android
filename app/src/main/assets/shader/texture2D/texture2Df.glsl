#version 300 es
precision mediump float;

uniform sampler2D u_texture;//当前纹理贴图

in vec2 o_uv;//纹理坐标
out vec4 fragColor;//输出到下一阶段的片段颜色属性

void main() {
    fragColor = texture(u_texture,o_uv);//对纹理图片进行采样
}
