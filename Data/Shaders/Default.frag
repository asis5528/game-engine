#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Color;
in vec2 TexCoords;
in vec3 normal;
in vec3 weight;
uniform bool bloom;
uniform bool BloomTexture;
uniform sampler2D GlowTexture;
uniform vec4 objectColor;
uniform int selected;

void main()
{    
	vec3 N = normalize(normal);
	N.z+=0.3;
	N.z = clamp(N.z,0.,1.);
	N.z*=0.7;

	FragColor = vec4(vec3(N.z),1.)*objectColor;
	FragColor = vec4(weight,1.);
	if(bloom){
		Color = objectColor;
		FragColor = objectColor;
		if(BloomTexture){
			
			vec4 glow =  texture(GlowTexture,TexCoords);
			Color =glow*vec4(15*glow.a);
		}
	}
	else{
		Color = vec4(0.,0.,0.,1.);
	}

	if(selected == 1){
		FragColor = vec4(1.0,1.0,1.,1.);
	}
	
}