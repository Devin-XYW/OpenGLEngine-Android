#version 300 es
precision highp float;

//11、马赛克效果滤镜

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

const vec2 mosaicSize = vec2(25.0,25.0);//马赛克大小

void main() {
    //获取纹理坐标对应的图像坐标
    vec2 intXY = out_uv * textSize;

    //floor为取整，将intXY除以马赛克的大小取整在乘以马赛克的大小
    //这样就使得这个马赛克区域内的像素都取自同一个像素
    vec2 XYMosaic = vec2(floor(intXY.x/mosaicSize.x)*mosaicSize.x
                        ,floor(intXY.y/mosaicSize.y)*mosaicSize.y);
    //恢复图像坐标为纹理坐标
    vec2 UVMosaic = XYMosaic / textSize;

    fragColor = YUVToRBG(UVMosaic);

}
