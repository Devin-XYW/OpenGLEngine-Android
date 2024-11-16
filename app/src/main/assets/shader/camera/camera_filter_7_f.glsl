#version 300 es
precision highp float;

//7.对图像进行4分屏滤镜效果

//对图像进行4分屏操作
//原理是在多个指定区域内对整个纹理进行下采样（缩小），从而实现整图像在多个区域内多次显示

uniform sampler2D uni_textureY;//Y分量
uniform sampler2D uni_textureU;//U分量
uniform sampler2D uni_textureV;//V分量

in vec2 out_uv;//纹理坐标
out vec4 fragColor;

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
    vec2 newTexCoord = out_uv;
    //对x方向纹理坐标进行2分量操作
    if(newTexCoord.x < 0.5){
        newTexCoord.x = newTexCoord.x * 2.0;
    }else{
        newTexCoord.x = (newTexCoord.x - 0.5) * 2.0;
    }

    //对y方向纹理坐标进行2分量分屏操作
    if(newTexCoord.y < 0.5){
        newTexCoord.y = newTexCoord.y * 2.0;
    }else{
        newTexCoord.y = (newTexCoord.y - 0.5) * 2.0;
    }

    fragColor = YUVToRBG(newTexCoord);
}
