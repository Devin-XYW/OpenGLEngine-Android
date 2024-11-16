#version 300 es
precision mediump float;

//转场动画对应的着色器地址：https://gl-transitions.com/editor/SimpleZoom
//实现放大然后转场的效果

uniform sampler2D texture_1;//当前纹理贴图1
uniform sampler2D texture_2;//当前纹理贴图2

in vec2 o_uv;//纹理坐标
out vec4 fragColor;//输出到下一阶段的片段颜色属性

uniform float u_offset;//一个0-1的uniform渐变量

const float zoom_quickness = 0.8;

vec2 zoom(vec2 uv,float amount){
    return 0.5 + ((uv - 0.5) * (1.0 - amount));
}

//转场函数
vec4 transition(vec2 uv){
    //clamp为最大值和最小值范围内：当x<min时返回min，当x>max时返回max，其余为x
    float nQuick = clamp(zoom_quickness,0.2,1.0);
    return mix(
        //y = smoothstep (float e0,float e1, float x)
        //如果e0<e1,若x<e0则y=0，若e0<=x<=e1则y=3*x^3-2*x^2,若x>e1则y=1
        texture(texture_1,zoom(uv,smoothstep(0.0,nQuick,u_offset))),
        texture(texture_2,uv),
        smoothstep(nQuick-0.2,1.0,u_offset)
    );
}

void main() {
    fragColor = transition(o_uv);
}
