#version 300 es
precision mediump float;

layout(location = 0) in vec3 a_position;//顶点坐标
layout(location = 1) in vec3 a_normal;//法线坐标
layout(location = 2) in vec2 a_texCoord;//纹理坐标

layout(location = 3) in vec3 tangent;//
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;//骨骼ID
layout(location = 6) in vec4 weights;//权重

out vec2 v_texCoord;//传递给片段着色器的纹理坐标

uniform mat4 u_MVPMatrix;//MVP矩阵
uniform mat4 u_ModelMatrix;//模型矩阵

uniform vec3 lightPos;//光源点位置
uniform vec3 lightColor;//光源颜色
uniform vec3 viewPos;//观察点位置坐标

const int MAX_BONES = 100;//骨头最大个数
const int MAX_BONE_INFLUENCE = 4;//一个骨骼最多被4个骨骼影响到
//影响该顶点的骨骼列表
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec3 ambient;//环境光
out vec3 diffuse;//漫反射
out vec3 specular;//镜面反射

void main() {

    v_texCoord = a_texCoord;

    //计算所有矩阵对顶点的影响最终值
    vec4 position = vec4(0.0f);
    for(int i=0;i<MAX_BONE_INFLUENCE;i++){
        if(boneIds[i] < 0){
            continue;
        }

        if(boneIds[i] >= MAX_BONES){
            //当前大于最大个数骨骼
            position = vec4(a_position,1.0f);
            break;
        }

        //挨个计算该顶点受影响的骨骼及其weight
        vec4 localPostion = finalBonesMatrices[boneIds[i]] * vec4(a_position,1.0f);
        position += localPostion * weights[i];
    }

    //使用MVP矩阵，计算当前坐标点传递给内置位置变量
    gl_Position = u_MVPMatrix * position;

    //计算位置坐标的世界坐标
    vec3 fragPos = vec3(u_ModelMatrix * position);

    //防止由于图形缩放等变换，这里将法向量进行变换处理
    vec3 normal = mat3(transpose(inverse(u_ModelMatrix))) * a_normal;

    //环境光
    float ambientStrength = 0.25;//环境光因子
    ambient = ambientStrength * lightColor;

    //漫反射光
    float diffuseStrength = 0.5;
    //计算当前法线方向向量，需要进行归一化处理
    vec3 unitNormal = normalize(normal);
    //当前光照方向，需要归一化
    vec3 lightDir = normalize(lightPos - fragPos);
    //计算当前光照方向与法线向量之间的夹角大小，用来计算光照强度
    float diff = max(dot(unitNormal,lightDir),0.0);
    //计算当前漫反射光
    diffuse = diffuseStrength * diff * lightColor;

    //计算镜面反射光
    float specularStrength = 0.3;
    //计算当前观察方向，需要进行归一化
    vec3 viewDir = normalize(viewPos - fragPos);
    //计算当前光照方向与法线向量的反射方向
    vec3 reflectDir = reflect(-lightDir, unitNormal);
    //计算当前反射光方向与观察方向向量之间的夹角大小，用来表示镜面反射光强度
    float spec = pow(max(dot(unitNormal,reflectDir),0.0),16.0);
    specular = specularStrength * spec * lightColor;
}