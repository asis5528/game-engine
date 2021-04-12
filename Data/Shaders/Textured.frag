#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Color;
in vec2 TexCoords;
in vec3 normal;

uniform bool bloom;
uniform bool BloomTexture;
uniform sampler2D DiffuseTexture;
uniform sampler2D GlowTexture;
uniform int selected;
uniform bool blending;

float LinearizeDepth(vec2 uv)
{
  float n = 0.001; // camera z near
  float f = 100.0; // camera z far
  float z = texture2D(DiffuseTexture, vec2(uv.x,1.-uv.y)).x;
  return (2.0 * n) / (f + n - z * (f - n));	
}
void main()
{    
	vec4 tex = texture(DiffuseTexture,TexCoords);
	tex.rgb = vec3(tex.rgb);
	//tex = vec4(LinearizeDepth(TexCoords));
	FragColor = tex;
	if(blending){
	FragColor.a = tex.r;
	}
	if(bloom){
		Color = tex;
		if(BloomTexture){
			Color = texture(GlowTexture,TexCoords)*5.;
		}
	}
	else{
		Color = vec4(0.,0.,0.,1.);
	}
	if(selected == 1){
		FragColor = vec4(1.0,1.0,1.,1.);
	}
	
}