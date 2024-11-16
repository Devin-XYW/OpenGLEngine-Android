#version 300 es
precision highp float;

//8.闪白效果滤镜

uniform sampler2D uni_textureY;//Y分量
uniform sampler2D uni_textureU;//U分量
uniform sampler2D uni_textureV;//V分量

in vec2 out_uv;//纹理坐标
out vec4 fragColor;

uniform float u_offset;//偏移量
uniform vec2 textSize;//纹理尺寸

const float PI = 3.1415926;

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

    //白色图层
    vec4 whiteMask = vec4(1.0,1.0,1.0,1.0);

    vec4 mask = YUVToRBG(out_uv);

    //白色图层的透明度是从0增加到1，在从1减少到0
    fragColor = mask * (1.0 - u_offset) + whiteMask * u_offset;

}