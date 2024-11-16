#version 300 es
precision mediump float;

//转场动画对应的着色器地址：https://gl-transitions.com/editor/pinwheel

uniform sampler2D texture_1;//当前纹理贴图1
uniform sampler2D texture_2;//当前纹理贴图2

in vec2 o_uv;//纹理坐标
out vec4 fragColor;//输出到下一阶段的片段颜色属性

uniform float u_offset;//一个0-1的uniform渐变量
uniform vec2 textSize;//纹理尺寸

const float speed = 2.0;

vec4 transition(vec2 uv){
    vec2 p = uv.xy / vec2(1.0).xy;
    float circPos = atan(p.y - 0.5,p.x - 0.5) + u_offset * speed;
    float modPos = mod(circPos,3.1415/4.0);
    float signed = sign(u_offset - modPos);

    vec4 result = mix(
        texture(texture_1,p),
        texture(texture_2,p),
        step(signed,0.5)
    );
    return result;
}

void main() {
    fragColor = transition(o_uv);
}
