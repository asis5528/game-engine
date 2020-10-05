#pragma once
#define PI 3.1415926535
#include <string>
#include "mesh.h"
using namespace std;
class Objects {
public:
	string name;
	unsigned int primitiveID;
	vector<unsigned int> textureID;
	Material mat;
	bool selected;
	glm::vec3 position;
	glm::vec3 scale = glm::vec3(1.0);
	glm::vec3 rotation;
	bool bloom = false;
	bool blending = false;
	glm::vec3 bloomColor = glm::vec3(0.0);
	glm::mat4 modelMatrix;
	glm::vec2 screenPos = glm::vec2(0.,0.);
	unsigned int animationID;
	std::vector<glm::mat4> boneMatrices;

	float animationTime;
	float blendFactor;
	unsigned int actionIndex;
	unsigned int blendIndex;
	bool animPlay = false;

	Objects(unsigned int primitiveIndex,string nam,Material &material) {
		primitiveID = primitiveIndex;
		mat = material;
		name = nam;
	
		
	} 
	Objects() {}

	void sendTransforms(Camera cam,Mesh &Mesh,Animation &anim) {
		mat.shad->use();
		mat.shad->setMat4("projection", cam.projMat);
		mat.shad->setMat4("view", cam.GetViewMatrix());

		glm::mat4 modelMatrix(1.0);

		modelMatrix = glm::translate(modelMatrix, glm::vec3(position[0], position[1], position[2]));
		modelMatrix = glm::rotate(modelMatrix, (float)(rotation[2] * (PI / 180)), glm::vec3(0, 0, 1));
		modelMatrix = glm::rotate(modelMatrix, (float)(rotation[1] * (PI / 180)), glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, (float)(rotation[0] * (PI / 180)), glm::vec3(1, 0, 0));


		modelMatrix = glm::scale(modelMatrix, scale);

		glm::vec4 pos = cam.projMat * cam.GetViewMatrix() * modelMatrix * glm::vec4(0., 0., 0., 1.);
		glm::vec3 ndcSpacePos = glm::vec3(pos.x / pos.w, pos.y / pos.w, pos.z / pos.w);
		screenPos = glm::vec2(ndcSpacePos.x * 0.5 + 0.5, ndcSpacePos.y * 0.5 + 0.5);
		mat.shad->setMat4("model", modelMatrix);
		//animation

		Animation* data = &anim;
		glm::mat4 parent(1.0);

		float range = data->actions[data->actionIndex].range.y - data->actions[data->actionIndex].range.x;

		//float animationTime = timer + data->actions[data->actionIndex].range.x;
		//timer += data->actions[data->actionIndex].range.x;
		if (animPlay) {
			animationTime += 0.5;
		}
		
		data->BoneTransform(animationTime,actionIndex,blendIndex,blendFactor);
		//	data->info.clear();
			//data->readAnimation((timer+ data->actions[data->actionIndex].range.x )/30, data->adata, parent);

		boneMatrices.clear();
		unsigned int j = 0;
		for (unsigned int i = 0; i < data->info.size(); i++) {
			if (data->info[i].name != Mesh.boneInfo[j].name) {
				//data->info.erase(data->info.begin() + i);


			}
			else {
				boneMatrices.push_back(data->info[i].transformedBone);
				j += 1;
				if (j > Mesh.boneInfo.size() - 1) {
					break;
				}
			}

		}

		mat.shad->setBool("hasAnimation", boneMatrices.size() > 0);
		if (data->adata.childAnimationData.size() > 0) {
			for (int i = 0; i < boneMatrices.size(); i++) {
				string loc = string("boneMat[") + std::to_string(i) + string("]");

				mat.shad->setMat4(loc, boneMatrices[i] * Mesh.boneInfo[i].BoneOffset);

			}
		}


	}
	void draw(Camera cam,Mesh &Mesh) {
		//glCullFace(GL_BACK);
		//glEnable(GL_CULL_FACE);
		//shader.use();
		mat.shad->use();
		mat.shad->setMat4("projection", cam.projMat);
		mat.shad->setMat4("view", cam.GetViewMatrix());

		glm::mat4 modelMatrix(1.0);

		modelMatrix = glm::translate(modelMatrix, glm::vec3(position[0], position[1], position[2]));
		modelMatrix = glm::rotate(modelMatrix, (float)(rotation[2] * (PI / 180)), glm::vec3(0, 0, 1));
		modelMatrix = glm::rotate(modelMatrix, (float)(rotation[1] * (PI / 180)), glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, (float)(rotation[0] * (PI / 180)), glm::vec3(1, 0, 0));


		modelMatrix = glm::scale(modelMatrix, scale);

		glm::vec4 pos = cam.projMat * cam.GetViewMatrix() * modelMatrix * glm::vec4(0., 0., 0., 1.);
		glm::vec3 ndcSpacePos = glm::vec3(pos.x / pos.w, pos.y / pos.w, pos.z / pos.w);
		screenPos = glm::vec2(ndcSpacePos.x * 0.5 + 0.5, ndcSpacePos.y * 0.5 + 0.5);
		mat.shad->setMat4("model", modelMatrix);
		
		mat.shad->setBool("hasAnimation", boneMatrices.size() > 0);
		if (boneMatrices.size() > 0) {
			for (int i = 0; i < boneMatrices.size(); i++) {
				string loc = string("boneMat[") + std::to_string(i) + string("]");

				mat.shad->setMat4(loc, boneMatrices[i] * Mesh.boneInfo[i].BoneOffset);

			}
		}

		Mesh.Draw(*mat.shad);
		
	}
	void drawCustom(Camera cam, Shader shad, Mesh& Mesh) {
		//glCullFace(GL_BACK);
		//glEnable(GL_CULL_FACE);
		shad.use();
		shad.setMat4("projection", cam.projMat);
		shad.setMat4("view", cam.GetViewMatrix());
		modelMatrix = glm::mat4(1.);
		glm::fquat d;
		glm::vec3 k = glm::eulerAngles(d);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(position[0], position[1], position[2]));
		modelMatrix = glm::rotate(modelMatrix, (float)(rotation[0] * (PI / 180)), glm::vec3(1, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, (float)(rotation[1] * (PI / 180)), glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, (float)(rotation[2] * (PI / 180)), glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, scale);
		shad.setMat4("model", modelMatrix);
		
		shad.setBool("hasAnimation", boneMatrices.size() > 0);
	
			for (int i = 0; i < boneMatrices.size(); i++) {
				string loc = string("boneMat[") + std::to_string(i) + string("]");

				shad.setMat4(loc, boneMatrices[i] * Mesh.boneInfo[i].BoneOffset);

			}
		
		Mesh.Draw(shad);
	
	}

};