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
	
	unsigned int boneIndex[4] = { 0,0,0,0 };
	float boneWeight[4] = { 0.0,0.0,0.0,0.0 };
	
};





class Mesh {
public:
	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::string name;
	std::vector<unsigned int> indices;
	glm::vec3 MaxBounds;
	glm::vec3 MinBounds;
	glm::vec3 Dimensions;
	unsigned int VAO;

	/*  Functions  */
	// constructor
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
	Mesh() {

	}

	void Draw(Shader shader,bool line  = 0)
	{
	

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

private:
	/*  Render data  */
	unsigned int VBO, EBO;

	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
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

		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIndex));

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneWeight));

		glBindVertexArray(0);
	}
};
#endif