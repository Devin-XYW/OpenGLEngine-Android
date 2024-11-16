#version 300 es
precision mediump float;

out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D texture_diffuse1;

in vec3 ambient;
in vec3 diffuse;
in vec3 specular;

void main() {
    //    vec4 objectColor = texture(texture_diffuse1, v_texCoord);
    //    //光照色乘于纹理颜色，等于最终的效果颜色
    //    vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);
    //    outColor = vec4(finalColor, 1.0);
    outColor = texture(texture_diffuse1, v_texCoord);
}
