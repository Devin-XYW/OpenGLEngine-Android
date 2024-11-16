#version 300 es
precision highp float;

//9.毛刺滤镜
// 毛刺滤镜实际上是图片撕裂加上颜色偏移，图片撕裂只能撕裂一小部分，否则图片可能都没办法分辨出来
// 将每一行像素随机偏移-1~1的距离，又因为只让一小部分撕裂，所以设定一个阈值，小于这个阈值才进行偏移
// 超过这个阈值则乘上一个缩小系数，保证图像的比较正常的显示
// 最终的效果：绝大部分都会进行微小的偏移，只有少量的行会进行较大偏移

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

//返回随机数
float rand(float n){
    //fract返回x的小数部分
    //返回 sin(n) * 43758.5453123
    //sin(n) * 极大值，带小数点，想要随机数算的比较低，乘的数就必须较大，噪声随机
    //如果想得到【0，1】范围的小数值，可以将sin * 1
    //如果只保留小数部分，乘以一个极大值
    return fract(sin(n) * 43758.5453123);
}

void main() {

    //最大抖动上限
    float maxJitter = 0.06;
    //红色颜色偏移
    float colorROffset = 0.01;
    //绿色颜色偏移
    float colorBOffset = -0.025;

    //像素随机偏移范围-1~1
    //先乘2，再减1的操作是为了得到[-1,1]范围内的随机值
    float jitter = rand(out_uv.y) * 2.0 - 1.0;
    //判断是否需要偏移，如果jitter范围 < 最大范围 * 振幅
    // abs(jitter) 范围【0，1】
    // maxJitter * amplitude 范围【0， 0.06】
    bool needOffset = abs(jitter) < (maxJitter * u_offset);

    //获取纹理x坐标，根据needoffset来计算它的x撕裂
    //needOffset = YES，则撕裂大
    //needOffset = NO，则撕裂小，需要降低撕裂 = *振幅*非常细微的数
    float textureX = out_uv.x + (needOffset ? jitter : (jitter * u_offset * 0.006));
    //获取纹理撕裂后的x，y坐标
    vec2 textureCoords = vec2(textureX,out_uv.y);

    //颜色偏移，获取3组颜色值
    //撕裂后的原图颜色
    vec4 mask = YUVToRBG(textureCoords);
    //根据撕裂计算后的纹理坐标，获取纹素
    vec4 maskR = YUVToRBG(textureCoords + vec2(colorROffset * u_offset,0.0));
    //根据撕裂计算后的纹理坐标，获取纹素
    vec4 maskB = YUVToRBG(textureCoords + vec2(colorBOffset * u_offset,0.0));

    //颜色主要撕裂红色和蓝色部分，所以只保留绿色
    fragColor = vec4(maskR.r,mask.g,maskB.b,mask.a);

}
