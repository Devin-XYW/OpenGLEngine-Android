#version 300 es
precision mediump float;

layout(location = 0) in vec3 a_position;//顶点坐标
layout(location = 1) in vec3 a_normal;//法线坐标
layout(location = 2) in vec2 a_texCoord;//纹理坐标

layout(location = 3) in vec3 tangent;//
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;//骨骼ID
layout(location = 6) in vec4 weights;//权重
layout(location = 7) in ivec4 boneIds_plus;//骨骼ID
layout(location = 8) in vec4 weights_plus;//权重

uniform mat4 u_MVPMatrix;//MVP矩阵
uniform mat4 u_ModelMatrix;//模型矩阵

const int MAX_BONES = 100;//骨头最大个数
const int MAX_BONE_INFLUENCE = 4;//一个骨骼最多被4个骨骼影响到
//影响该顶点的骨骼列表
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 uv;//传递给片段着色器的纹理坐标
out vec3 fragPos;
out mat3 vary_tbnMatrix;
out vec3 normal;

void main() {
    uv = a_texCoord;
    //计算所有矩阵对顶点的影响最终值
    vec4 position = vec4(0.0f);
    //把所有影响的骨骼的换算矩阵，乘于原始的顶点坐标，加和，得到动画之后的新的顶点坐标
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] >= 0){
            vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(a_position,1.0f);
            position += localPosition * weights[i];
        }
    }
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds_plus[i] >= 0){
            vec4 localPosition = finalBonesMatrices[boneIds_plus[i]] * vec4(a_position,1.0f);
            position += localPosition * weights_plus[i];
        }
    }

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
