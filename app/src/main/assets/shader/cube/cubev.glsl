#version 300es

layout(location = 0) in vec3 a_position;//顶点坐标
layout(location = 1) in vec2 a_uv;//纹理坐标

uniform mat4 u_mat;//变换矩阵
out vec2 o_uv;//纹理坐标

void main() {

    o_uv = a_uv;
    gl_Position = u_mat * vec4(a_position,1.0);

}
