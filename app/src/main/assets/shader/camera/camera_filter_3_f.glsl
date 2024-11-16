#version 300 es
precision highp float;

//3.对画面进行缩放的圆效果滤镜

//对图像进行所缩放的圆的效果实现主要依赖偏移量来动态改变圆半径的大小
// 在半径区域内对纹理采样显示图像，在半径区域外返回一个固定的颜色
// distance是GLSL中内建函数，用于计算两点之间的距离
// 需要注意的是，在计算之前首先需要将纹理坐标系转换为图片坐标系，
//原因在于纹理横纵坐标的取值范围为[0,1]。从数值上看纹理的横纵方向长度相同，但是在OpenGL采样时，图像的宽高比往往不是1
// 这就导致数值相同的横纵坐标，对应不同的采样权重，出现预期绘制圆形而实际上却绘制除椭圆的情况

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

    vec2 imgTex = out_uv* textSize;//将纹理坐标转换为图片坐标
    float r = (u_offset) * textSize.x;//计算当前偏移量处理后的圆半径
    if(distance(imgTex,vec2(textSize.x / 2.0,textSize.y / 2.0)) < r){
        fragColor = YUVToRBG(out_uv);
    }else{
        fragColor = vec4(1.0,1.0,1.0,1.0);
    }
//    fragColor = YUVToRBG(out_uv);
}
