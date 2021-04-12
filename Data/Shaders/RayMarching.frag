#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Color;
in vec2 TexCoords;
in vec3 normal;
in vec3 weight;
in vec3 modelMatPos;
uniform bool bloom;
uniform bool BloomTexture;
uniform sampler2D GlowTexture;
uniform vec4 objectColor;
uniform int selected;
uniform vec3 camPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
float map(vec3 p){
float d = length(p)-5.0;
return d;
}


vec4 march(vec3 ro,vec3 rd){
float t = 0.0;
	for(int i = 0;i<200;i++){
	vec3 p = ro+rd*t;
	float h = map(p);
	if(h<0.001) break;
	t+=h;
	if(t>120.) break;
	
	
	}
	if(t>120.) t=-1.;
	return vec4(t,0.,0.,0.);

}


void main()
{    
	vec3 N = normalize(normal);
	N.z+=0.3;
	N.z = clamp(N.z,0.,1.);
	N.z*=0.7;


	vec2 uv= TexCoords;
	vec3 ro =camPos;
	vec3 rd = normalize(vec3(uv,-1.));
	rd = normalize(modelMatPos-camPos);
	float  t = march(ro,rd).x;
	FragColor = vec4(camPos,1.);
	if(t>0.0){
	        vec3 eyeFwd = vec3(0.,0.,-1.) * mat3(view * model);
  
            float eyeHitZ = -t * dot(rd, eyeFwd);
            float p10 = projection[2].z;
    float p11 = projection[3].z;
    float ndcDepth = -p10 + -p11 / eyeHitZ;
    float dep = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;
	vec3 p = ro+rd*t;
	float zc = ( (view * model)*projection * vec4( p, 1.0 ) ).z;
float wc = ( (view * model)*projection * vec4( p, 1.0 ) ).w;
            gl_FragDepth = ((zc/wc)+1.0)*0.5;


	FragColor = vec4(1.,0.,0.,0.);
	}else{
	discard;
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