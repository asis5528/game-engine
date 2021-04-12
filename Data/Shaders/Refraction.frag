#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Color;
in vec2 TexCoords;
in vec3 normal;
in vec4 viewPos;
in vec3 modelMatPos;
in vec4 projCoord;
uniform bool bloom;
uniform bool BloomTexture;
uniform sampler2D DiffuseTexture;
uniform sampler2D GlowTexture;
uniform int selected;
uniform bool blending;
uniform vec3 camPos;
void main()
{    

vec2 uv = gl_FragCoord.xy/vec2(1512.,1512.);
///vec2 n = vec2();
vec3 V = normalize(camPos-modelMatPos);
float dd = dot(V,normal);
vec3 refracted = refract(V, normalize(normal), 1.0);
//refracted
	vec4 tex;
	tex.rgb =   texture(DiffuseTexture,refracted.xy).rgb;
	tex = vec4(dd);
	FragColor = tex;
	FragColor.a = 1.;
	if(selected == 1){
		FragColor = vec4(1.0,1.0,1.,1.);
	}
	
}