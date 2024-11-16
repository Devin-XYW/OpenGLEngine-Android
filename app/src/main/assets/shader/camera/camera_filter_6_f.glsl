#version 300 es
precision highp float;

//6.旋转的圆滤镜效果

//旋转的圆滤镜特效：对某一半径内的所有像素，按照偏移量转换成的角度进行旋转
//半径之外的像素正常渲染

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

const float PI = 3.141592653;
void main() {
    //纹理坐标转换为图片坐标
    vec2 imgTex = out_uv * textSize;
    //设置半径为图片宽度的0.3倍
    float r = 0.5 * textSize.x;

    //取圆心为图片中心点
    if(distance(imgTex,vec2(textSize.x / 2.0,textSize.y / 2.0)) < r){

        vec2 tranTex = out_uv - 0.5;
        vec2 imgTranTex = tranTex * textSize;
        float len = length(imgTranTex);
        float angle = 0.0;

        angle = acos(imgTranTex.x / len);
        if(tranTex.y < 0.0){
            angle *= -1.0;
        }

        angle -= u_offset;

        imgTranTex.x = len * cos(angle);
        imgTranTex.y = len * sin(angle);

        vec2 newTexCoors = imgTranTex / textSize + 0.5;

        fragColor = YUVToRBG(newTexCoors);
    }else{
        fragColor = YUVToRBG(out_uv);
    }


}
