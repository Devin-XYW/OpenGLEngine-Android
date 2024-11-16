#version 300 es
precision mediump float;

layout(location = 0) in vec3 a_position;//顶点坐标
layout(location = 1) in vec3 a_normal;//法线坐标
layout(location = 2) in vec2 a_texCoord;//纹理坐标

layout(location = 3) in vec3 tangent;//
layout(location = 4) in vec3 bitangent;

uniform mat4 u_MVPMatrix;//MVP矩阵
uniform mat4 u_ModelMatrix;//模型矩阵

out vec2 uv;//传递给片段着色器的纹理坐标
out vec3 fragPos;
out mat3 vary_tbnMatrix;
out vec3 normal;

void main() {
    uv = a_texCoord;
    //计算所有矩阵对顶点的影响最终值
    vec4 position = vec4(a_position,1.0f);
    //使用MVP矩阵，计算当前坐标点传递给内置位置变量
    gl_Position = u_MVPMatrix * position;

    //计算法线
    normal = mat3(transpose(inverse(u_ModelMatrix))) * a_normal;
    vec3 a_tangent = normalize(mat3(transpose(inverse(u_ModelMatrix))) * tangent);
    vec3 a_bitangent = normalize(mat3(transpose(inverse(u_ModelMatrix))) * bitangent);
    vary_tbnMatrix = mat3(a_tangent, a_bitangent, normal);  //得到的是TBN空间矩阵  传给片元着色器
    //计算位置坐标的世界坐标
    fragPos = vec3(u_ModelMatrix * position);
}
