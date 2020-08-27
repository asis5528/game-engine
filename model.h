#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader_m.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

class Model
{
public:
	/*  Model Data */
	vector<string> names;
	vector<Mesh> meshes;
	vector<glm::mat4> transformations;
	vector<glm::vec3> positions;
	vector<glm::vec3> rotations;
	vector<glm::vec3> scales;
	string directory;
	bool gammaCorrection;

	/*  Functions   */
	// constructor, expects a filepath to a 3D model.
	Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path); ;
	}
	Model() {

	}

	// draws the model, and thus all its meshes
	void Draw(Shader shader,bool line = 0)
	{
		if (!line) {
			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i].Draw(shader);
		}
		else {
			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i].Draw(shader,1);
		}

		}
		
	unsigned int LoadTexture(const char* path)
	{
		string filename = string(path);
		

		unsigned int textureID;
		glGenTextures(1, &textureID);
		stbi_set_flip_vertically_on_load(0);
		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		float n = 1.0;
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}
	
	unsigned int loadCubemap(vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}
	

private:
	/*  Functions   */
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	std::string getExtensionName(std::string filePath)
	{
	
		std::size_t dotPos = filePath.rfind('.');
		return filePath.substr(dotPos+1,filePath.size());
	}
	inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from)
	{
		glm::mat4 to;


		to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
		to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
		to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
		to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

		return to;
	}
	void loadModel(string const& path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		string fileName = getExtensionName(path);
		bool fbx = false;
		bool obj = false;
		if (fileName == "FBX" || fileName == "fbx") {
			fbx = true;
		}
		else if (fileName == "OBJ" || fileName == "obj") {
			obj = true;
		}
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
		// check for errors
		float k =  scene->mAnimations[0]->mDuration;
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene,fbx, obj);
	
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene,bool fbx,bool obj)
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			bool whyyoubullyme = scene->HasAnimations();
			std::cout << "show me what you got" << " " << whyyoubullyme << "\n";
			const aiMatrix4x4 mat =  node->mTransformation;
			transformations.push_back(aiMatrix4x4ToGlm(&mat));
			 aiVector3D position;
			 aiQuaternion rotation;
				aiVector3D scale;
			node->mTransformation.Decompose(scale, rotation, position);
			glm::vec3 pos = glm::vec3(position.x, position.y, position.z);
			glm::vec3 sca = glm::vec3(scale.x, scale.y, scale.z);
			if (fbx) {
				sca /= 40.;
			}
			glm::fquat qrot;
			qrot.x = rotation.x;
			qrot.y = rotation.y;
			qrot.z = rotation.z;
			qrot.w = rotation.w;
			glm::vec3 rot = glm::degrees(glm::eulerAngles(qrot));
			if (!obj) {
				rot.x -= 90;
			}
			
			if (std::fabs(rot.z) >= 89.99) {
				float r = rot.z;
			
				//rot.x += 180.f;
		//	rot.y = 180.f - rot.y;
				//rot.z += 180.f;
			}
			//rot.x = rot.x -90;
			rotations.push_back(rot);
			positions.push_back(pos);
			scales.push_back(sca);
			
			node->mTransformation;
			names.push_back(node->mName.C_Str());
			std::cout << node->mName.C_Str() << "\n";
			meshes.push_back(processMesh(mesh, scene));
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			
			processNode(node->mChildren[i], scene,fbx,obj);
		}

	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// data to fill
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		glm::vec3 Max = glm::vec3(0.);
		glm::vec3 Min = glm::vec3(0.);
		
		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// positions
			vector.x = mesh->mVertices[i].x;
			float x = vector.x;
			
			
			vector.y = mesh->mVertices[i].y;
			float y = vector.y;
			vector.z = mesh->mVertices[i].z;
			float z = vector.z;
			if (x > Max.x) {
				Max.x = x;
			}
			if (y > Max.y) {
				Max.y = y;
			}
			if (z > Max.z) {
				Max.z = z;
			}
			if (x < Min.x) {
				Min.x = x;
			}
			if (y < Min.y) {
				Min.y = y;
			}
			if (z < Min.z) {
				Min.z = z;
			}

			vertex.Position = vector;
			// normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			int num = mesh->mNumBones;
			mesh->mBones[i];
			vertex.Normal = vector;
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		
			vertices.push_back(vertex);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		mesh->mNumVertices;
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		for (int i = 0; i < mesh->mNumBones; i++) {
			//auto bone = mesh->mBones[i];
			//auto id = bone->mName.C_Str();
			//std::cout << "bone's name is : "<< i << "\n";

			for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
				int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
				float Weight = mesh->mBones[i]->mWeights[j].mWeight;
				for (int k = 0; k < 4; k++) {
				//	std::cout << vertices[VertexID].boneWeight[k] << "wei\n";
					if (vertices[VertexID].boneWeight[k] == 0.0) {
						vertices[VertexID].boneIndex[k] = i;
						vertices[VertexID].boneWeight[k] = Weight;
						//std::cout << k << "IDs\n";
						break;
					}
					
				}
				
				//std::cout << mesh->mNumVertices << " Num\n";
				//std::cout << VertexID << " VertexID\n";
				//std::cout << Weight << " Weight\n";
				float k = 1.0;
			}
		}


		// process materials
		
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN


		// return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices);
	}

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	
};
#endif