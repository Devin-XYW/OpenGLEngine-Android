#version 300 es
precision mediump float;

uniform sampler2D u_texture;
in vec2 o_uv;
out vec4  fragColor;

void main(void)
{
    fragColor = texture(u_texture,o_uv);
}
