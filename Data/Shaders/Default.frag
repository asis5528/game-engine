#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Color;
in vec2 TexCoords;
in vec3 normal;
in vec3 weight;
in vec3 modelMatPos;;
uniform bool bloom;
uniform bool BloomTexture;
uniform mat4 invprojection;
uniform mat4 viewinverse;
uniform sampler2D GlowTexture;
uniform vec4 objectColor;
uniform int selected;
uniform sampler2D DiffuseTexture;


vec3 WorldPosFromDepth(float depth,vec2 uv) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invprojection * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = viewinverse * viewSpacePosition;

    return worldSpacePosition.xyz;
}

float LinearizeDepth(vec2 uv)
{

  float n = 0.001; // camera z near
  float f = 10.0; // camera z far
  float z = texture2D(DiffuseTexture, vec2(uv.x,uv.y)).x;
  return (2.0 * n) / (f + n - z * (f - n));	
}
void main()
{    
	vec3 N = normalize(normal);
	N.z+=0.3;
	N.z = clamp(N.z,0.,1.);
	N.z*=0.7;
	FragColor = vec4(vec3(N.z),1.)*objectColor;
	if(objectColor.r<0.99){
		vec2 uv = modelMatPos.xy/(modelMatPos.z+0.0001);
		
		uv*=0.5;
		uv+=0.5;
		uv.x=1.-uv.x;
	//	uv.y=1.-uv.y;
		//uv = clamp(uv,vec2(0.0),vec2(1.0));
		float dep = LinearizeDepth(uv);
		vec3 world = WorldPosFromDepth(texture2D(DiffuseTexture,uv).x,uv);
		float dis = length(modelMatPos);
		if(dis>length(world)){
		discard;
		}
		float c = smoothstep(0.3,0.2,length(uv-vec2(0.5,0.5)));
		//FragColor=vec4(modelMatPos,1.);
		FragColor=vec4(world,1.);
	}
	
	

	//FragColor = vec4(weight,1.);
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