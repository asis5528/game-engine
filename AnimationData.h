#pragma once
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
struct KeyTranslation {
	glm::vec3 position;
	float time;
};

struct KeyScale {
	glm::vec3 scale;
	float time;
};

struct KeyRotation {
	glm::quat rotation;
	float time;
};

struct AnimationTransformation {
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	float time;
};

class AnimationData {
public:
	string name = "";
	//std::vector<glm::vec3> Translation;
	//std::vector<glm::vec3> Scale;
	//std::vector<glm::quat> Rotation;
	

	std::vector<AnimationTransformation> animationTransformation;
	std::vector<AnimationData> childAnimationData;
	unsigned int index;
	

	

	
};
