#version 300 es
precision highp float;

//10.旋涡滤镜

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

    float radius = 600.0;//半径
    float angle = 0.8;//旋转参考值
    vec2 center = vec2(textSize.x / 2.0, textSize.y / 2.0);//圆心坐标
    vec2 tc = out_uv * textSize;//将纹理坐标转换为图片坐标
    tc -= center;//计算当前纹理坐标点到圆心距离
    float dist = length(tc);//计算当前纹理坐标点到圆心距离
    if (dist < radius) {
        float percent = (radius - dist) / radius;
        float theta = percent * percent * angle * 8.0;
        float s = sin(theta);
        float c = cos(theta);
        tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
    }
    tc += center;

    fragColor = YUVToRBG(tc/textSize);
}
