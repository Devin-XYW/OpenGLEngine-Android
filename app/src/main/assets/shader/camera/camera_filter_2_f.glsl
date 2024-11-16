#version 300 es
precision highp float;

//2.对图像进行网格化特效

//对图像网格化分割：将纹理划分为多个网格，然后根据一个偏移量动态改变网格线的宽度
// 其中mod和floor为GLSL的内建函数，分别表示取模和取整
// 需要注意的是，计算之前需要将纹理坐标系转换为图片坐标系，保证网格没有被拉伸

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

    vec2 imgTexCoord = out_uv * textSize;//将纹理坐标系转换为图片坐标系
    float sideLength = textSize.y / 6.0;//网格的边长
    float maxOffset = 0.15 * sideLength;//设置网格线宽度的最大值
    float x = mod(imgTexCoord.x, floor(sideLength));
    float y = mod(imgTexCoord.y, floor(sideLength));

    float offset = u_offset * maxOffset;

    if(offset <= x
    && x <= sideLength - offset
    && offset <= y
    && y <= sideLength - offset)
    {
        fragColor = YUVToRBG(out_uv);
    }
    else
    {
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}
