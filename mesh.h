#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader_m.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
	

};

struct BoneInfo
{
	string name;
	glm::mat4 BoneOffset;
	
	

};

struct VertexBoneData
{
	unsigned int IDs[4] = { 0,0,0,0 };
	float Weights[4] = { 0.0,0.0,0.0,0.0 };

	VertexBoneData()
	{
		Reset();
	};

	void Reset()
	{
		//ZERO_MEM(IDs);
		//ZERO_MEM(Weights);
	}

	void AddBoneData(unsigned int BoneID, float Weight) {
		for (unsigned int  i = 0; i < sizeof(IDs) / sizeof(IDs[0]); i++) {
			if (Weights[i] == 0.0) {
				IDs[i] = BoneID;
				Weights[i] = Weight;
				return;
			}
		}
	};
};



class Mesh {
public:
	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<VertexBoneData> bones;
	std::string name;
	std::vector<unsigned int> indices;
	std::vector <glm::mat4> bonePoseMatrix;
	std::vector< BoneInfo> boneInfo;
	Animation animation;
	glm::vec3 MaxBounds;
	glm::vec3 MinBounds;
	glm::vec3 Dimensions;
	unsigned int VAO;
	float timer = 0.0;
	bool animPlay = false;

	/*  Functions  */
	// constructor
	Mesh() {

	}
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
	{
		this->vertices = vertices;
		this->indices = indices;
		for (auto& v : vertices) {
			if (v.Position.x > MaxBounds.x) {
				MaxBounds.x = v.Position.x;
			}
			if (v.Position.y > MaxBounds.y) {
				MaxBounds.y = v.Position.y;
			}
			if (v.Position.z > MaxBounds.z) {
				MaxBounds.z = v.Position.z;
			}
			if (v.Position.x < MinBounds.x) {
				MinBounds.x = v.Position.x;
			}
			if (v.Position.y < MinBounds.y) {
				MinBounds.y = v.Position.y;
			}
			if (v.Position.z < MinBounds.z) {
				MinBounds.z = v.Position.z;
			}
		}
		Dimensions = MaxBounds+abs(MinBounds);
		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}
	void AnimationInit(std::vector<Vertex> vertices, std::vector<unsigned int> indices,std::vector<VertexBoneData> bones) {
		this->vertices = vertices;
		this->indices = indices;
		this->bones = bones;
		for (auto& v : vertices) {
			if (v.Position.x > MaxBounds.x) {
				MaxBounds.x = v.Position.x;
			}
			if (v.Position.y > MaxBounds.y) {
				MaxBounds.y = v.Position.y;
			}
			if (v.Position.z > MaxBounds.z) {
				MaxBounds.z = v.Position.z;
			}
			if (v.Position.x < MinBounds.x) {
				MinBounds.x = v.Position.x;
			}
			if (v.Position.y < MinBounds.y) {
				MinBounds.y = v.Position.y;
			}
			if (v.Position.z < MinBounds.z) {
				MinBounds.z = v.Position.z;
			}
		}
		Dimensions = MaxBounds + abs(MinBounds);
		if (bones.size() > 0) {
			this->m_hasBones = true;
		}
		
		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}

	void Draw(Shader shader,bool line  = 0)
	{
		Animation* data = &animation;
		glm::mat4 parent(1.0);

		if (animPlay) {
			timer += 0.01*24*data->actions[data->actionIndex].speed;
		}
		float range = data->actions[data->actionIndex].range.y - data->actions[data->actionIndex].range.x;
		timer = fmod(timer,range);
		//float animationTime = timer + data->actions[data->actionIndex].range.x;
		//timer += data->actions[data->actionIndex].range.x;
		
		data->info.clear();
		data->readAnimation((timer+ data->actions[data->actionIndex].range.x )/24, data->adata, parent);

		std::vector<glm::mat4> boneMatrices;
		unsigned int j = 0;
		for (unsigned int i = 0; i < data->info.size();i++) {
			if (data->info[i].name != boneInfo[j].name) {
				//data->info.erase(data->info.begin() + i);
				

			}
			else {
				boneMatrices.push_back(data->info[i].transformedBone);
				j += 1;
				if (j > boneInfo.size() - 1) {
					break;
				}
			}

		}

		shader.setBool("hasAnimation", data->adata.childAnimationData.size()>0);
		if(data->adata.childAnimationData.size() > 0){
			for (int i = 0; i < boneMatrices.size(); i++) {
				string loc = string("boneMat[") + std::to_string(i) + string("]");
			
				shader.setMat4(loc, boneMatrices[i]*boneInfo[i].BoneOffset);
			
			}
		}

		glBindVertexArray(VAO);
		if (line) {
			glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
		}
		else {
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		}
		
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
	//	glActiveTexture(GL_TEXTURE0);
	}
	bool hasAnimation() {
		return this->m_hasBones;
	}

private:
	/*  Render data  */
	unsigned int VBO, EBO;
	bool m_hasBones = false;
	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh()
	{
		unsigned int VBO1;
		// create buffers arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &VBO1);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		std::cout << "size of vertex while buffer" << vertices.size() << "\n";
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		if(m_hasBones){
			glBindBuffer(GL_ARRAY_BUFFER, VBO1);
			glBufferData(GL_ARRAY_BUFFER, bones.size() * sizeof(VertexBoneData), &bones[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(5);
			glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexBoneData), (void*)0);
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)16);
		}
		glBindVertexArray(0);
	}
};
#endif