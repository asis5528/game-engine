#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Color;
in vec2 TexCoords;
in vec3 normal;
in vec3 modelMatPos;
in vec4 viewPos;
uniform vec3 CamPos;
uniform bool bloom;
uniform vec3 color;
uniform vec3 bloomColor;
uniform int useTexture;
uniform int selected;
uniform float time;
uniform sampler2D tex;
void main()
{    
	vec3 viewDirection = normalize(modelMatPos-CamPos);
	vec3 P = viewPos.xyz;
	vec3 V = normalize(-viewDirection);
	vec3 N = normalize(normal);
	vec3 light = vec3(0.,0.,5.);

	float factor =dot(N,light);
	float fre = smoothstep(0.0,1.0,1.-dot(N,V));
	float k =  step(0.5,fre);
	//vec4 col = vec3(fre);
    //FragColor =vec4(col*color,1.);
	vec4 objecttexture;
	if(useTexture ==1){
		objecttexture = texture(tex,vec2(TexCoords.x,TexCoords.y));
		// = col;
	}else{
	objecttexture = vec4(0.,0.,0.,1.);
	}
	N.z+=0.3;
	N.z = clamp(N.z,0.,1.);
	N.z*=0.7;

	FragColor =vec4(mix(N.z*vec3(1.),objecttexture.xyz,float(useTexture)),1.);
	if(bloom){
	Color = vec4(bloomColor,0.)+objecttexture*10.*vec4(vec3(sin(time*5.)),1.);
	FragColor = vec4(bloomColor,0.)+objecttexture;
	
	}
	else{
	Color = vec4(0.,0.,0.,1.);
	}
	if(selected == 1){
	FragColor = vec4(1.0,1.0,1.,1.);
	}
		//FragColor.a = 1.;
	
	
}