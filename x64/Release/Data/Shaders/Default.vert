#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in ivec4 aBoneIndex;
layout (location = 6) in vec4 aBoneWeight;

out vec2 TexCoords;
out vec3 normal;
out vec3 modelMatPos;
out vec4 viewPos;
out vec3 weight;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 boneMat[100];
uniform bool hasAnimation;
void main()
{
	normal = mat3(transpose(inverse(model))) *aNormal;
	//weight = vec3(aBoneIndex.xyz);
    TexCoords = aTexCoords;    
	modelMatPos = vec3(model * vec4(aPos, 1.0));
	viewPos = (view*vec4(aPos,1.));
	
	vec4 PosL;
	PosL =vec4(aPos, 1.0);
	if(hasAnimation){
	
		mat4 BoneTransform = boneMat[aBoneIndex[0]] * aBoneWeight[0];
		BoneTransform += boneMat[aBoneIndex[1]] * aBoneWeight[1];
		BoneTransform += boneMat[aBoneIndex[2]] * aBoneWeight[2];
		BoneTransform += boneMat[aBoneIndex[3]] * aBoneWeight[3];
		 PosL =BoneTransform* vec4(aPos, 1.0);
	}else{
	PosL =vec4(aPos, 1.0);
	}
	
    gl_Position = projection * view * model * PosL;
}
