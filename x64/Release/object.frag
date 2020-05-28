#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 normal;
in vec3 modelMatPos;
in vec4 viewPos;
uniform sampler2D tex;
uniform vec3 CamPos;
uniform vec3 color;
void main()
{    
	


	vec3 viewDirection = normalize(modelMatPos-CamPos);
	vec3 P = viewPos.xyz;
	vec3 V = normalize(-viewDirection);
	vec3 N = normalize(normal);
	float factor =dot(N,V);
	float fre = smoothstep(0.0,1.0,1.-dot(N,V));
	float k =  step(0.5,fre);
	vec3 col = vec3(fre);
    //FragColor =vec4(col*color,1.);
	FragColor =vec4(col,1.);
	
}