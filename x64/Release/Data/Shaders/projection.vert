#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 pos;
out vec4 tc;
out vec4 tc2;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 checkpoint1;
uniform vec3 checkpoint2;


void main()
{


	
	tc =vec4(aPos-checkpoint1,1.);
	tc2 = vec4(aPos-checkpoint2,1.);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
