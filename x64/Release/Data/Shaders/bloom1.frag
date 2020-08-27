#version 330 core
out vec4 FragColor;
in vec2 uv;
in vec2 blurTexCoords[11];
uniform sampler2D tex0;


void main()
{
    FragColor = vec4(0.0);
    FragColor += texture(tex0, blurTexCoords[0]) * 0.0093;
    FragColor += texture(tex0, blurTexCoords[1]) * 0.028002;
    FragColor += texture(tex0, blurTexCoords[2]) * 0.065984;
    FragColor += texture(tex0, blurTexCoords[3]) * 0.121703;
    FragColor += texture(tex0, blurTexCoords[4]) * 0.175713;
    FragColor += texture(tex0, blurTexCoords[5]) * 0.198596;
    FragColor += texture(tex0, blurTexCoords[6]) * 0.175713;
    FragColor += texture(tex0, blurTexCoords[7]) * 0.121703;
    FragColor += texture(tex0, blurTexCoords[8]) * 0.065984;
    FragColor += texture(tex0, blurTexCoords[9]) * 0.028002;
    FragColor += texture(tex0, blurTexCoords[10]) * 0.0093;

    //FragColor= texture(tex0,uv);
}