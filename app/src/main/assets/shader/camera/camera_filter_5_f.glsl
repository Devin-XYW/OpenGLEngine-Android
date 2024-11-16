#version 300 es
precision highp float;

//5.灵魂出窍滤镜效果

//灵魂出窍滤镜效果：根据偏移量offset，进行scale变换纹理坐标，分别进行采样后
//再按照混合系数进行加权混合
//实际上是两层图像的叠加，并且上面一层图像随着时间的推移，会逐渐放大且不透明度逐渐降低

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

const float MAX_ALPHA = 0.5;//透明度上限
const float MAX_SCALE = 0.8;//最大放大图像上限

void main() {

    //根据偏移量计算透明度混合系数：alpha
    float alpha = MAX_ALPHA * (1.0-u_offset);
    //根据偏移量计算缩放系数：scale
    float scale = 1.0 + u_offset * MAX_SCALE;

    //对上层图片进行缩放操作
    //将顶点坐标对应的纹理坐标的x值到纹理中点的距离，缩小一定比例
    float scale_x = 0.5+(out_uv.x - 0.5) / scale;
    float scale_y = 0.5+(out_uv.y - 0.5) / scale;

    //进行放大后的纹理坐标
    vec2 scaleCoord = vec2(scale_x,scale_y);

    //经过上述计算，我们得到了两个纹理颜色值，上层经过缩放和透明度处理的maskColor颜色值
    //和原始纹理originColor颜色值
    vec4 maskColor = YUVToRBG(scaleCoord);
    vec4 originColor = YUVToRBG(out_uv);

    //加权混合：在GLSL中实现颜色混合方程=originColor * (1.0-alpha) + maskColor * alpha
    // 计算最终的颜色赋值给片元着色器的内置变量
    fragColor = originColor * (1.0 - alpha) + maskColor * alpha;
}
