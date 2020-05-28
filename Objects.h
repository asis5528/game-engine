#pragma once
#define PI 3.1415926535
#include <string>
#include "mesh.h"
using namespace std;
class Objects {
public:
	string name;
	Shader shader;
	Mesh mesh;
	bool selected;
	glm::vec3 position;
	glm::vec3 scale = glm::vec3(1.0);
	glm::vec3 rotation;
	Texture texture;
	glm::mat4 modelMatrix;
	glm::vec2 screenPos = glm::vec2(0.,0.);
	Objects(Mesh mes,Shader shad,string nam) {
		name = nam;
		shader = shad;
		mesh = mes;
	} 
	Objects() {

	}
	void draw(Camera cam) {
		//glCullFace(GL_BACK);
		//glEnable(GL_CULL_FACE);
		shader.use();
		shader.setMat4("projection", cam.projMat);
		shader.setMat4("view", cam.GetViewMatrix());
		
		glm::mat4 modelMatrix(1.0);
		
		modelMatrix = glm::translate(modelMatrix, glm::vec3(position[0], position[1], position[2]));
		modelMatrix = glm::rotate(modelMatrix,(float) (rotation[0] *(PI / 180)), glm::vec3(1, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, (float)(rotation[1] * (PI / 180)), glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, (float)(rotation[2] * (PI / 180)), glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, scale);

		glm::vec4 pos = cam.projMat * cam.GetViewMatrix() * modelMatrix * glm::vec4(0., 0., 0., 1.);
		glm::vec3 ndcSpacePos = glm::vec3(pos.x / pos.w, pos.y / pos.w, pos.z / pos.w);
		screenPos = glm::vec2(ndcSpacePos.x*0.5+0.5, ndcSpacePos.y*0.5+0.5);
		shader.setMat4("model", modelMatrix);
		shader.setVec3("CamPos", cam.Position);
		if (this->selected) {
			shader.setVec3("color", glm::vec3(1., 0., 0.));
			
		}
		else {
			shader.setVec3("color", glm::vec3(1., 1., 1.));
		}
		mesh.Draw(shader);
		
	}
	void drawCustom(Camera cam, Shader shad) {
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
		//shad.setVec3("CamPos", cam.Position);
		
		mesh.Draw(shader);
	
	}

};