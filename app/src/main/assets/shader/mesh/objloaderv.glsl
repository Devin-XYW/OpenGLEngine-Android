#version 300 es

layout(location = 0) in   vec3 a_position;//顶点坐标
layout(location = 1) in   vec2 a_texturCoord;//纹理坐标
layout(location = 2) in   vec3 a_normal;//法线向量
layout(location = 3) in   vec3 a_tangent;//T分量：切线
layout(location = 4) in   vec3 a_bitangent;//B分量：双切线

uniform  mat4 u_projectionMatrix;//投影矩阵
uniform  mat4 u_viewMatrix;//观察矩阵
uniform mat4 u_modelMatrix;//模板矩阵

out  vec4 vary_pos;//记录模板矩阵*顶点坐标的值，表示当前的坐标变换，传递给片段着色器
out  vec2 vary_texCoord;//传递给片段着色器的纹理坐标
out  mat3 vary_tbnMatrix;//计算当前的TBN矩阵


void main(void)
{
    //当前顶点坐标=投影矩阵 * 观察矩阵 * 模板矩阵
    mat4 mv_matrix = u_viewMatrix * u_modelMatrix;
    gl_Position = u_projectionMatrix * mv_matrix * vec4(a_position,1.0);

    vary_texCoord = a_texturCoord;//纹理坐标传递给片段着色器

    //当前变换矩阵=模板矩阵*顶点向量，传递给片段着色器
    vary_pos = u_modelMatrix *  vec4(a_position,1.0);

    //如果模型矩阵执行不了等比缩放，法线向量就不会垂直于对应的表面，光照就会被破坏
    //利用法线矩阵可以解决,法线向量*模型矩阵的逆矩阵的转置：normal = mat3(transpose(inverse(model)) * aNormal
    vec3 normal = normalize(mat3(transpose(inverse(u_modelMatrix))) * a_normal);
    vec3 tangent = normalize(mat3(transpose(inverse(u_modelMatrix))) * a_tangent);
    vec3 bitangent = normalize(mat3(transpose(inverse(u_modelMatrix))) * a_bitangent);
    //将当前TBN矩阵传递给片段着色器
    vary_tbnMatrix = mat3(tangent, bitangent, normal);

}
