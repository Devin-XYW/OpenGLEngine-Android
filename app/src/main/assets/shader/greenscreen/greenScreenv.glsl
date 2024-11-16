#version 300 es

layout(location = 0) in vec3 a_position;//顶点坐标属性
layout(location = 1) in vec2 a_uv;//纹理坐标属性

uniform mat4 u_mat;//MVP矩阵

out vec2 o_uv;//传递为片段着色器的纹理属性

void main() {
    o_uv = a_uv;
    gl_Position = u_mat * vec4(a_position,1.0);
}
