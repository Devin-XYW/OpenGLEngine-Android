#version 300 es
precision mediump float;

uniform sampler2D u_texture;//纹理
in vec2 o_uv;//纹理坐标
out vec4 fragColor;

void main() {
    vec4 texColor = texture(u_texture,o_uv);
    //将贴图中透明的纹理部分（图片中a分量<0.1），不进行渲染
    if(texColor.a < 0.1){
        discard;
    }

    fragColor = texColor;
}
