#version 300 es
precision highp float;

//4.分色偏移效果滤镜

//分色偏移滤镜：基于原纹理坐标进行偏移，分别采样后再按照RGBA通道合成，组成一个新的颜色

uniform sampler2D uni_textureY;//Y分量
uniform sampler2D uni_textureU;//U分量
uniform sampler2D uni_textureV;//V分量

in vec2 out_uv;//纹理坐标
out vec4 fragColor;

uniform float u_offset;//偏移量
uniform vec2 textSize;//纹理尺寸

//将YUV数据转化为RGB数据
vec4 YUVToRBG(vec2 uv){
    vec3 yuv;
    vec3 rgb;
    yuv.x = texture(uni_textureY, uv).r;
    yuv.y = texture(uni_textureU, uv).r - 0.5;
    yuv.z = texture(uni_textureV, uv).r - 0.5;
    rgb = mat3( 1,1,1, 0,-0.39465,2.03211,1.13983,-0.58060,0) * yuv;
    return vec4(rgb,1.0);
}

void main() {
    //放大图形的上限
    float maxScale = 1.1;
    //颜色偏移的步长
    float offset = 0.02;

    //颜色偏移值：0-0.02
    vec2 offsetCoords = vec2(offset,offset) * u_offset;
    //缩放比例：1.0-1.1
    float scale = 1.0 + (maxScale - 1.0) * u_offset;

    //放大后的纹理坐标
    //以下这种向量相加减的方式，等价于灵魂出鞘滤镜中单个计算x、y坐标再组合为纹理坐标的方式
    vec2 scaleTextureCoords = vec2(0.5,0.5) + (out_uv - vec2(0.5,0.5)) / scale;

    //获取三组颜色：颜色偏移计算可以随意，只需要偏移量很小就行
    //原始颜色 + offset
    vec4 maskR = YUVToRBG(scaleTextureCoords + offsetCoords);
    //原始颜色 - offset
    vec4 maskB = YUVToRBG(scaleTextureCoords - offsetCoords);
    //原始颜色
    vec4 mask = YUVToRBG(scaleTextureCoords);

    //从3组颜色中分别取出，红色R、绿色G、蓝色B，透明度A填充到内置颜色变量中
    fragColor = vec4(maskR.r,maskB.g,mask.b,mask.a);
}
