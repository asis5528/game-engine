#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 normal;
in vec3 modelMatPos;
in vec4 viewPos;
in vec4 texCoords;
in vec4 tc;
uniform sampler2D tex;
uniform sampler2D tex1;
uniform vec3 CamPos;
uniform float timer;
vec4 toLatLong(vec4 v) // incoming vertex after MVP mult. 
{
    float val;
    vec4 o = vec4(1.0);

    // radius, theta, phi
    float r = sqrt(pow(v.x, 2.0) + pow(v.y, 2.0) + pow(v.z, 2.0));
    float theta = acos(v.z/r);
    float phi   = atan(v.y/v.x);

    o.x = clamp(0.5 + phi/(2.0* 3.1415926), 0.0, 1.0);
    o.y = clamp(theta/ 3.1415926, 0.0, 1.0);
    //o.z = (r * cos(theta))/v.z;
    return o; // final vertex pos. gl_position.
}
void main()
{    
	

		 vec2 longitudeLatitude =toLatLong(texCoords.xzyw).xy;
		 longitudeLatitude.x = 1.-abs(longitudeLatitude.x);
		 longitudeLatitude.x*=2.0001;
		 vec4 newpos = tc;
		// newpos.y+=4.;
		 vec2 uv =toLatLong(newpos.xzyw).xy;
		 uv.x = 1.-abs(uv.x);
		 uv.x*=2.0001;
		 vec4 col1 = texture(tex,longitudeLatitude);
		  vec4 col2 = texture(tex1,uv);
		  float factor = clamp(timer,0.,1.);
	FragColor =mix(col1,col2,factor);
	
	FragColor =vec4(1.,0.,0.,1.);
	
}