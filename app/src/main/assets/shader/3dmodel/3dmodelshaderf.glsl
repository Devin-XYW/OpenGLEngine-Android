#version 300 es
precision highp float;

out vec4 outColor;

in vec2 uv;
in vec3 normal;
in mat3 vary_tbnMatrix;
in vec3 fragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform vec3 lightPos;//光源点位置
uniform vec3 lightColor;//光源颜色
uniform vec3 viewPos;//观察点位置坐标

vec3 CalcDirLight(vec3 normal_world,vec3 viewDir)
{
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 diffuse = texture(texture_diffuse1,uv).rgb;
    //环境光
    float ambientStrength = 0.5;//环境光因子
    vec3 ambient_light = ambientStrength * diffuse * lightColor;

    //漫反射
    //漫反射光
    float diffuseStrength = 0.5;
    float dif = max(dot(normal_world,lightDir),0.0);
    vec3 diffuse_light = diffuseStrength * dif * diffuse * lightColor;

    //高光
    float specularStrength = 0.3;
    vec3 specular = texture(texture_specular1,uv).rgb;
    vec3 reflectDir = reflect(-lightDir,normal_world);
    float spec = pow(max(dot(normal_world,reflectDir),0.0),16.0);
    vec3 specular_light = specularStrength * spec * specular * lightColor;

    return (ambient_light + diffuse_light + specular_light);
}

void main() {
    vec3 normal_ = normalize(normal * 2.0 - 1.0);
    normal_ = normalize(vary_tbnMatrix * normal);

    vec3 worldNormal = normalize(normal_);
    vec3 viewDir = normalize(viewPos - fragPos);
    //定向光
    highp vec3 result = CalcDirLight(worldNormal,viewDir);
    outColor = vec4(result,1.0);
//    outColor = texture(texture_diffuse1, uv);
}