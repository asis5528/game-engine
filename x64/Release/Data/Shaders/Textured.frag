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

void main()
{    
	vec4 tex = texture(DiffuseTexture,TexCoords);
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