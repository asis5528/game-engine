#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 normal;
out vec3 modelMatPos;
out vec4 viewPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
normal = mat3(transpose(inverse(model))) *aNormal;
    TexCoords = aTexCoords;    
	modelMatPos = vec3(model * vec4(aPos, 1.0));
	viewPos = (view*vec4(aPos,1.));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
