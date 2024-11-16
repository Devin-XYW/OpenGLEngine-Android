#version 300es
precision mediump float;

uniform sampler2D utexture;//纹理工具
in vec2 o_uv;//顶点着色器传递过来的纹理坐标
out vec4 fragColor;//当前片段点颜色

void main() {
    fragColor = texture(utexture,o_uv);
}
