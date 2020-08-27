#version 330 core
out vec4 FragColor;

vec2 sp(vec3 r){
	vec2 uv;
	uv.x = atan( r.x, r.z );
	uv.y = acos( r.y );
	uv /= vec2( 2 * 3.1415, 3.1415);
	return uv;
}


in vec4 tc;
in vec4 tc2;
uniform sampler2D tex;
uniform sampler2D tex1;


uniform float factor;
void main()
{   


		
		vec3 r = tc.xyz;
		vec3 r2 = tc2.xyz;
		vec2 st = sp(normalize(vec3(-tc.x,tc.y,tc.z)));
		vec2 st2 = sp(normalize(vec3(-tc2.x,tc2.y,tc2.z)));
	  vec4 text1 = texture(tex,st);
	  vec4 text2 = texture(tex1,st2);
	FragColor =mix(text2,text1,factor);
	}