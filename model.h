#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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


class Model
{
public:
	/*  Model Data */
	vector<string> names;
	Animation sceneAnimation;
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

	map<string, int> m_BoneMapping;
	glm::mat4 globalInverseTransform;
	std::vector<AnimationData> animationData;
	AnimationData dat;
	vector<BoneInfo> m_BoneInfo;
	int m_NumBones = 0;
	const aiScene* m_pScene;
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
		//float k =  scene->mAnimations[0]->mDuration;
		//globalInverseTransform = aiMatrix4x4ToGlm(&scene->mRootNode->mTransformation);
		//glm::inverse(globalInverseTransform);
		
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		m_pScene = scene;
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));
		//aiAnimation* animation = scene->mAnimations[0];
		
		//for (int i = 0; i < animation->mNumChannels; i++) {
		//	std::cout << string(animation->mChannels[i]->mNodeName.data) << "\n";
	//	}

	//	const aiNodeAnim* pNodeAnim = FindNodeAnim(animation, string(scene->mRootNode->mChildren->mName.data));

		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene,fbx, obj);
		AnimationData animData;
		if (m_pScene->HasAnimations()) {
			readAnimation(scene->mRootNode, &animData);
		}
		if (m_pScene->HasAnimations()) {
			Animation animation;
			animation.ticksperSec = m_pScene->mAnimations[0]->mTicksPerSecond;
			animation.duration = m_pScene->mAnimations[0]->mDuration;
			if (fileName == "FBX" || fileName == "fbx") {
				//animation.fps = 1.0;
			}
			animation.adata = animData;
			animation.initAction();
			sceneAnimation = animation;
		}
	
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
			if (obj) {
				rot.x -= 90;
			}
			else {
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
		float n;
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
		
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		} 
		std::vector< VertexBoneData> Bones;
		
	
		if(mesh->HasBones()){
			Bones.resize(mesh->mNumVertices);
		}
		for (int i = 0; i < mesh->mNumBones; i++) {
			
			int BoneIndex = 0;
			string BoneName(mesh->mBones[i]->mName.data);
			if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
				// Allocate an index for a new bone
				BoneIndex = m_NumBones;
				m_NumBones++;
				BoneInfo bi;
				m_BoneInfo.push_back(bi);
				
				m_BoneInfo[BoneIndex].BoneOffset = aiMatrix4x4ToGlm(&mesh->mBones[i]->mOffsetMatrix);
				aiVector3D position;
				aiQuaternion rotation;
				aiVector3D scale;
				mesh->mBones[i]->mOffsetMatrix.Decompose(scale, rotation, position);
				m_BoneInfo[BoneIndex].name = BoneName;

				m_BoneMapping[BoneName] = BoneIndex;
			}
			else {
				BoneIndex = m_BoneMapping[BoneName];
			}
	
			for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
				unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
				float Weight = mesh->mBones[i]->mWeights[j].mWeight;
				Bones[VertexID].AddBoneData(BoneIndex, Weight);
			}
		}
	
		
		
		Mesh m;
		m.bones = Bones;
		m.AnimationInit(vertices, indices, Bones);
		m.boneInfo = m_BoneInfo;
		
		
	
		// process materials
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN
		// return a mesh object created from the extracted mesh data
		
	
		return m;
	}
	
	

	void readAnimation(const aiNode* pNode,AnimationData* aData) {
		string NodeName(pNode->mName.data);
		const aiAnimation* pAnimation = m_pScene->mAnimations[0];
		const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
		glm::mat4 RotationMatrix(1.0);
		unsigned int animationDataIndex = 1000;
		glm::mat4 globalTransform(1.0);
		if (pNodeAnim) {
		//	aiVector3D Scaling;
			//CalcInterpolatedScaling(Scaling, 0.0, pNodeAnim);
			
		
		
				//animationData[animationDataIndex].ticksperSec = pAnimation->mTicksPerSecond;
				//animationData[animationDataIndex].duration = pAnimation->mDuration;
				AnimationData data;
				for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys; i++) {
					AnimationTransformation trans;
					aiVector3D  po= pNodeAnim->mPositionKeys[i].mValue;
					aiVector3D  sc = pNodeAnim->mScalingKeys[i].mValue;
					 aiQuaternion q = pNodeAnim->mRotationKeys[i].mValue;
					trans.position = glm::vec3(po.x,po.y,po.z);
					 trans.scale = glm::vec3(sc.x, sc.y, sc.z);
					 glm::quat qu;
					 qu.w = q.w;
					 qu.x = q.x;
					 qu.y= q.y;
					 qu.z = q.z;
					 trans.rotation = qu;
					 trans.time = pNodeAnim->mRotationKeys[i].mTime;
					 data.animationTransformation.push_back(trans);
					 data.name = NodeName;
					// data.ticksperSec = pAnimation->mTicksPerSecond;
					// data.duration =  pAnimation->mDuration;
					
				
			}
				aData->childAnimationData.push_back(data);

			

	
			
			
			
		}
		
		
		for (int i = 0; i < pNode->mNumChildren; i++) {
			std::cout << "parent : " << pNode->mName.data << " --- child : " << pNode->mChildren[i]->mName.data << "\n";
			
				if (pNodeAnim) {
					readAnimation( pNode->mChildren[i], &aData->childAnimationData[aData->childAnimationData.size() - 1]);
					
				}
				else {
					readAnimation( pNode->mChildren[i], aData);
				}
				
			
			
			
		}
	}
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
	{
		for (int i = 0; i < pAnimation->mNumChannels; i++) {
			const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

			if (string(pNodeAnim->mNodeName.data) == NodeName) {
				return pNodeAnim;
			}
		}

		return NULL;
	}















	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	
};
#endif