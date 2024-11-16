#version 300 es
precision highp float;

//光照相关系数
struct Light
{
    vec3 m_pos;//光源点
    vec3 m_ambient;//环境光照颜色
    vec3 m_diffuse;//漫反射光照颜色
    vec3 m_specular;//镜面反射光照颜色
    //衰减变量
    float m_c;
    float m_l;
    float m_q;
};

uniform Light myLight;//当前光照结构

uniform sampler2D   texture_diffuse;//纹理采样器：颜色纹理图
uniform sampler2D   texture_normal;//纹理采样器：法线纹理图
uniform sampler2D   texture_specular;//纹理采样器：高光纹理图

uniform float       m_shiness;//反射光强度因子

uniform vec3       u_viewPos;//观察点位置

in vec4 vary_pos;//顶点着色器传递过来的，记录模板矩阵*顶点坐标的值,表示世界坐标中的片段位置
in vec2 vary_texCoord;//顶点着色器传递过来的，纹理坐标
in  mat3 vary_tbnMatrix;//顶点着色器传递过来的，TBN矩阵

out vec4 fragColor;

void main(void)
{

    //在世界空间计算光照

    //首先计算当前法线信息，法线向量*TBN矩阵，并进行归一化处理
    //法线的x，y，z存储在纹理中，代替颜色的rgb值
    vec3 normal = texture(texture_normal,vary_texCoord).rgb;
    //将法线向量转换到-1到1的空间
    normal = normalize(normal * 2.0 - 1.0);
    //将法向量*TBN并进行归一化
    normal = normalize(vary_tbnMatrix * normal);

    //计算光照
    //首先计算光源点到顶点的距离
    float dist = length(myLight.m_pos - vary_pos.xyz);
    //计算光照的衰减值
    float attenuation = 1.0f / (myLight.m_c + myLight.m_l * dist + myLight.m_q *dist * dist);

    //光颜色 * 模型颜色贴图（通过texture进行采样）
    vec3 ambient = myLight.m_ambient * vec3(texture(texture_diffuse , vary_texCoord).rgb);

    //漫反射光照
    vec3 lightDir = normalize(myLight.m_pos - vary_pos.xyz);//计算光照与顶点的方向向量并归一化
    float diff = max(dot(normal , lightDir) , 0.0f);//点乘计算法向量和光照向量的夹角
    //光颜色 * 夹角大小 * 模型颜色贴图
    vec3 diffuse = myLight.m_diffuse * diff * vec3(texture(texture_diffuse , vary_texCoord).rgb);

    //镜面反射
    float specular_strength = 0.5;//反射强度
    vec3 viewDir = normalize(u_viewPos - vary_pos.xyz);//观察方向向量
    vec3 reflectDir = reflect(-lightDir , normal);//光照反射光线向量
    //计算观察方向与反射光方向夹角值参数，m_shiness为反射光强度因子，可以控制效果
    float spec =  pow(max(dot(viewDir , reflectDir) , 0.0f) , m_shiness);
    //反射强度 * 反射光颜色 * 夹角参数
    vec3 sepcular = specular_strength * myLight.m_specular * spec;
    //vec3 sepcular = specular_strength* myLight.m_specular * spec * vec3(texture(texture_specular , vary_texCoord).rgb);

    vec3 result = ambient  + diffuse + sepcular ;
    fragColor = vec4(result,1.0f) ;

}
