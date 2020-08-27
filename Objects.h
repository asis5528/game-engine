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

	Objects(unsigned int primitiveIndex,string nam,Material &material) {
		primitiveID = primitiveIndex;
		mat = material;
		name = nam;
	
		
	} 
	Objects() {}
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
		modelMatrix = glm::rotate(modelMatrix,(float)(rotation[0] *(PI / 180)), glm::vec3(1, 0, 0));
		
		
		modelMatrix = glm::scale(modelMatrix, scale);

		glm::vec4 pos = cam.projMat * cam.GetViewMatrix() * modelMatrix * glm::vec4(0., 0., 0., 1.);
		glm::vec3 ndcSpacePos = glm::vec3(pos.x / pos.w, pos.y / pos.w, pos.z / pos.w);
		screenPos = glm::vec2(ndcSpacePos.x*0.5+0.5, ndcSpacePos.y*0.5+0.5);
		mat.shad->setMat4("model", modelMatrix);
		/*
		shader.setVec3("CamPos", cam.Position);
		for (int i = 0; i < sceneLight.lights.size(); i++) {
			shader.setVec3(string("LightPos") + "[" + std::to_string(i) + "]", sceneLight.lights[i].position);
		}

		if (this->selected) {
			
			shader.setVec3("color", glm::vec3(1., 0., 0.));
		}
		else {
			shader.setVec3("color", glm::vec3(1., 1., 1.));
		}
		shader.setBool("bloom", bloom);
		if (bloom) {
			shader.setVec3("bloomColor", bloomColor);
		}
		shader.setFloat("time", glfwGetTime());
		*/
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
		Mesh.Draw(shad);
	
	}

};