#pragma once
class SceneLoader {
public:

	static Mesh LoadPrimitive(string path) {
		string fullname = path+".pribln";
		ifstream iff(fullname);
		string nam;
		string n;
		
		//iff >> x >> y>> z;
		iff >> n >> nam;
		
		vector<float> vertices;
		vector<unsigned int> indices;
		vector<float> boneDatas;
		vector<string> boneList;
		vector<BoneInfo> boneInfo;
		string line;
		int state = 0;
		while (std::getline(iff, line)) {
			string n;
			iff >> n;
		
			if (state == 1) {
				std::cout << "loading vertices/n";
				string line2;
				std::getline(iff, line2);
				istringstream iss(line2);
				float vertex;
				vertex = std::stof(n);
				vertices.push_back(vertex);
				while (iss >> vertex) {
					vertices.push_back(vertex);
				}

				state = 2;


			}
			else if (state == 2) {
				std::cout << "loading indices/n";
				string line2;
				std::getline(iff, line2);
				istringstream iss(line2);

				int index;

				index = std::stoi(n);
				indices.push_back(index);
				while (iss >> index) {
					indices.push_back(index);
				}

				state = 3;

			}
			else if (state == 3) {
				std::cout << "loading bone/n";
				string line2;
				std::getline(iff, line2);
				istringstream iss(line2);
				float boneData;
				boneData = std::stof(n);
				boneDatas.push_back(boneData);
				while (iss >> boneData) {
					boneDatas.push_back(boneData);
				}

				state = 4;

			}
			else if (state == 4) {
				std::cout << "loading bone offset matrix /n";
				string line2;
				std::getline(iff, line2);
				istringstream iss(line2);
				string boneName;
				//boneList.push_back(n);
				BoneInfo bi;
				bi.name = n;
				float matrix[16];
				for (int i = 0; i < 16; i++) {
					float value;
					iss >> value;
					matrix[i] = value;

				}
				bi.BoneOffset = glm::make_mat4(matrix);
				boneInfo.push_back(bi);
				while (iss >> boneName) {
					bi.name = boneName;
					float matrix[16];
					for (int i = 0; i < 16; i++) {
						float value;
						iss >> value;
						matrix[i] = value;

					}
					bi.BoneOffset = glm::make_mat4(matrix);
					boneInfo.push_back(bi);

				}

				float k = 0.1;
			}
			if (n == "vertexDatas") {
				state = 1;
				
			}
		}
		vector<Vertex> verts;
		vector<VertexBoneData> bones;
		for (int i = 0; i < vertices.size() / 8; i++) {
			Vertex v;
			v.Position.x = vertices[i * 8 + 0];
			v.Position.y = vertices[i * 8 + 1];
			v.Position.z = vertices[i * 8 + 2];
			v.Normal.x = vertices[i * 8 + 3];
			v.Normal.y = vertices[i * 8 + 4];
			v.Normal.z = vertices[i * 8 + 5];
			v.TexCoords.x = vertices[i * 8 + 6];
			v.TexCoords.y = vertices[i * 8 + 7];
			v.Tangent = glm::vec3(0);
			v.Bitangent = glm::vec3(0);
			verts.push_back(v);
		}

		for (int i = 0; i < boneDatas.size() / 8; i++) {
			VertexBoneData bone;
			bone.IDs[0] = boneDatas[i * 8 + 0];
			bone.IDs[1] = boneDatas[i * 8 + 1];
			bone.IDs[2] = boneDatas[i * 8 + 2];
			bone.IDs[3] = boneDatas[i * 8 + 3];
			bone.Weights[0]= boneDatas[i * 8 + 4];
			bone.Weights[1] = boneDatas[i * 8 + 5];
			bone.Weights[2] = boneDatas[i * 8 + 6];
			bone.Weights[3] = boneDatas[i * 8 + 7];
			bones.push_back(bone);
		}

		float k = 1.;
		
		Mesh m;
		m.boneInfo = boneInfo;
		m.AnimationInit(verts, indices, bones);
		//////////Come back////////////
		//Animation animation;
		//loadAnimation(path, animation);
		//m.animation = animation;
		//m.animation.initAction();
		//m.AnimationInit(verts, indices, bones);
		return m;

	}
	static void loadAnimation(string path,Animation &animation) {
		string fullname = path + ".anibln";
		ifstream iff(fullname);
		string line;
		//AnimationData adata;
	
		string parent;
		string child;
		//std::getline(iff, line);
		//iff >> parent; iff >> child;
		string j;
		iff >> animation.name;
		float duration;
		iff >> duration;
		float tick;
		iff >> tick;
		unsigned int keyframeSize = 0;
		iff >> keyframeSize;
		unsigned int BoneIndex = 0;
		
		animation.duration = duration;
		animation.ticksperSec = tick;
		string useless;
		iff >> useless;
		unsigned int actionSize;
		iff >> actionSize;
		std::vector<AnimationAction> actions;
		
		for (unsigned int i = 0; i < actionSize; i++) {
			std::cout << "lets start animation : " <<i <<"\n";
			string name;
			float x, y;
			iff >> name;
			iff >> x;
			iff >> y;
			AnimationAction ac;
			ac.name = name;
			ac.range = glm::vec2(x, y);
			actions.push_back(ac);
			
		}
		std::cout << "lets start animation" << "\n";
		animation.actions = actions;
		unsigned int state = 0;
		std::vector<std::vector<AnimationTransformation>> AnimationTransformationData;
		std::vector<AnimationTransformation> AnimT;
		std::vector<AnimationTransformation>* AT = &AnimT;
		
		while (std::getline(iff, line)) {
			
				string firstValue;
				iff >> firstValue;
				if (firstValue.size() > 0) {
				
				if (firstValue.at(0) == '#') {
					//state = 1;
					
					iff >> firstValue;
					//std::cout << "bones " << AnimationTransformationData.size() << "\n";
					AnimationTransformationData.resize(AnimationTransformationData.size() + 1);
					AT = &AnimationTransformationData[AnimationTransformationData.size()-1];
					
				}
				if (firstValue == "AnimationTree") {
					
					iff >> parent; iff >> child;
					animation.adata.name = parent;
					AnimationData childdata;
					childdata.name = child;
					childdata.animationTransformation = AnimationTransformationData[BoneIndex];
					BoneIndex+=1;
					animation.adata.childAnimationData.push_back(childdata);
					state = 1;
					iff >> firstValue;
				}
				
				//if()
				if (state == 0) {

					AnimationTransformation at;
					glm::vec3 pos;
					pos.x = std::stof(firstValue);
					iff >> pos.y;
					iff >> pos.z;
					at.position = pos;
					glm::vec3 scale;
					iff >> scale.x;
					iff >> scale.y;
					iff >> scale.z;
					at.scale = scale;
					glm::quat rot;
					iff >> rot.w;
					iff >> rot.x;
					iff >> rot.y;
					iff >> rot.z;
					at.rotation = rot;
					float time;
					iff >> time;
					at.time = time;
					string line;
					AT->push_back(at);
					//iff >> line;
					//iff >> line;
					float j = 5;

					
				}
				else if (state == 1) {
					string parent = firstValue;
					string child;
					iff >> child;
					bool node = searchNode(animation.adata, parent, child, AnimationTransformationData[BoneIndex]);
					//node->animationTransformation = AnimationTransformationData[BoneIndex];
					BoneIndex += 1;
					//AnimationData childData;
					//childData.name = child;
					//parentdata->childAnimationData.push_back(childdata);
				//	float n = 1;
				}

			}
			
		
		}
		
		float k = 1.334;


	}
	static bool searchNode(AnimationData& adata, string Parentname,string Childname,std::vector<AnimationTransformation> &ATT) {
		if (Parentname == adata.name) {
			AnimationData childData;
			childData.name = Childname; 
			childData.animationTransformation = ATT;
			adata.childAnimationData.push_back(childData);
			return true;

			
		}
		for (int i = 0; i < adata.childAnimationData.size(); i++) {
			searchNode(adata.childAnimationData[i], Parentname, Childname,ATT);
		}
	
		
	}
	static void savePrimitive(vector<Primitives> &allPrimitives,string &path) {
		std::cout << "haha" << "\n";
		for (Primitives& p : allPrimitives) {
			ofstream myfile;
			string finalname = path+"\\" + p.name + ".pribln";
			std::cout << finalname << "\n";
			myfile.open(finalname);
			myfile << "Name " << p.name << "\n";
			myfile << "vertexDatas \n";
			for (unsigned int i = 0; i < p.mesh.vertices.size(); i++) {
				Vertex v = p.mesh.vertices[i];
				
				myfile << v.Position.x << " " << v.Position.y << " " << v.Position.z << " ";
				myfile << v.Normal.x << " " << v.Normal.y << " " << v.Normal.z << " ";
				myfile << v.TexCoords.x << " " << v.TexCoords.y << " ";
				
			}
			
		
			myfile << " \n";
			myfile << "indices \n";
			for (auto i : p.mesh.indices) {
				myfile << i << " ";
			}
			myfile << " \n";
			if (p.mesh.hasAnimation()) {
				myfile << "boneDatas \n";
				for (unsigned int i = 0; i < p.mesh.vertices.size(); i++) {
					VertexBoneData vb = p.mesh.bones[i];
					if (p.mesh.hasAnimation()) {
						myfile << vb.IDs[0] << " " << vb.IDs[1] << " " << vb.IDs[2] << " " << vb.IDs[3] << " ";
						myfile << vb.Weights[0] << " " << vb.Weights[1] << " " << vb.Weights[2] << " " << vb.Weights[3] << " ";
					}
				}
				myfile << " \n";
				
				myfile << "boneList \n";
				saveBoneListMesh(myfile, p.mesh);
				myfile.close();

			}
			//myfile.close();
			/*
			if (p.mesh.animation.adata.childAnimationData.size() > 0) {
				
				ofstream myfile;
				string finalname = path + "\\" + p.name + ".anibln";
				std::cout << finalname << "\n";
				//myfile << "AnimationData" << "\n";
				
				myfile.open(finalname);
				myfile << "Animation ";
				myfile << p.mesh.animation.duration << " " << p.mesh.animation.ticksperSec << "\n";
				saveBoneAnimation(myfile, p.mesh.animation.adata);
				myfile << "AnimationTree" << "\n";
				//myfile << "hehe";
				saveBoneTree(myfile, p.mesh.animation.adata);
				
				myfile.close();
			}
			*/
		}
	}


	/////////////THIRD PARTY EXPORTER/////////////////
	static void savejsPrimitive(vector<Objects> objects,vector<Primitives>& allPrimitives, string& path, std::vector<Texture>& textures) {
		
		
		for (unsigned int i = 0; i < objects.size(); i++) {
			ofstream myfile;
			string finalname = path;
			std::cout << finalname << "\n";
			myfile.open(finalname);
			//	myfile << "Name " << p.name << "\n";
			//	myfile << "vertexDatas \n";
			Objects object = objects[i];

			Primitives p = allPrimitives[object.primitiveID];

			myfile << "export class BooleanGameGeometry  {\n";
			myfile << "getGeometry(decode){\n";
			myfile << "const positions = new Float32Array([";
			myfile << p.mesh.vertices[0].Position.x << "," << p.mesh.vertices[0].Position.y << "," << p.mesh.vertices[0].Position.z;
			for (unsigned int i = 1; i < p.mesh.vertices.size(); i++) {
				Vertex v = p.mesh.vertices[i];
				myfile << "," << p.mesh.vertices[i].Position.x << "," << p.mesh.vertices[i].Position.y << "," << p.mesh.vertices[i].Position.z;
			}
			myfile << "]);\n";

			myfile << "const uvs = new Float32Array([";

			for (unsigned int i = 0; i < p.mesh.vertices.size(); i++) {
				Vertex v = p.mesh.vertices[i];
				myfile << p.mesh.vertices[i].TexCoords.x << "," << p.mesh.vertices[i].TexCoords.y << ",";
			}
			myfile << "]);\n";

			myfile << "const normals = new Float32Array([";

			for (unsigned int i = 0; i < p.mesh.vertices.size(); i++) {
				Vertex v = p.mesh.vertices[i];
				myfile << p.mesh.vertices[i].Normal.x << "," << p.mesh.vertices[i].Normal.y << "," << p.mesh.vertices[i].Normal.z << ",";
			}
			myfile << "]);\n";


			//myfile << "const tData = new Uint8Array([";


			



			int numChannels = 0;
			string tformat = "THREE.RGBFormat";
			string boolrgba = "true";
			if (textures[object.mat.DiffuseTextureID].format == GL_RED){
				numChannels = 1;
			}
			else if (textures[object.mat.DiffuseTextureID].format == GL_RGB) {
				boolrgba = "false";
				numChannels = 3;
			}
			else if (textures[object.mat.DiffuseTextureID].format == GL_RGBA) {
				tformat = "THREE.RGBAFormat";
				numChannels = 4;
			}
				
			const int numberOfPixels = textures[object.mat.DiffuseTextureID].width * textures[object.mat.DiffuseTextureID].height * numChannels;
			unsigned char* pixels = new unsigned char[numberOfPixels];


			glBindTexture(textures[object.mat.DiffuseTextureID].type, textures[object.mat.DiffuseTextureID].id);

			glGetTexImage(GL_TEXTURE_2D, 0, textures[object.mat.DiffuseTextureID].format, GL_UNSIGNED_BYTE, pixels);
			unsigned int size = sizeof(pixels);
			for (unsigned int i = 0; i < numberOfPixels; i++) {
				//unsigned int val = pixels[i];
				//myfile << val << ",";
			}
		//	myfile << "]);\n";
			const char* jpegFilename = (Tools::getPathName(finalname) + "//save.png").c_str();
			stbi_write_jpg(jpegFilename, textures[object.mat.DiffuseTextureID].width, textures[object.mat.DiffuseTextureID].height, numChannels, pixels, 50);
			std::ifstream file(jpegFilename, std::ios::binary | std::ios::ate);
			std::streamsize bufsize = file.tellg();
			file.seekg(0, std::ios::beg);
			
			std::vector<char> buffer(bufsize);
			if (file.read(buffer.data(), bufsize))
			{
				/* worked! */
			}
			
			file.close();
			myfile << "var tData =new Uint8Array([";
			//myfile << "var tData = \" ";
			for (unsigned int i = 0; i < buffer.size(); i++) {
				unsigned int val = (uint8_t)buffer[i];
				myfile << val<<",";
				 //myfile << buffer[i];
			}
			//myfile << "\";\n";
				myfile << "]);\n";

				myfile << "var imageData = decode(tData,{formatAsRGBA:";
				myfile << boolrgba;
				myfile<<"});\n";

			myfile << "const texture = new THREE.DataTexture(imageData.data," << "imageData.width" << "," << "imageData.height" << ","<< tformat <<");\n";
			myfile << "var material = new THREE.MeshBasicMaterial({ map: texture});";


			myfile << " const indices = [";

			for (auto i : p.mesh.indices) {
				myfile << i << ",";
			}
			myfile << "];\n";
			myfile << "const geometry = new THREE.BufferGeometry();\n";
			myfile << " geometry.setAttribute('position',new THREE.BufferAttribute(positions, 3));\n";
			myfile << " geometry.setAttribute('normal',new THREE.BufferAttribute(normals, 3));\n";
			myfile << " geometry.setAttribute('uv',new THREE.BufferAttribute(uvs, 2));\n";
			myfile << "geometry.setIndex(indices);\n";
			myfile << "var mesh = new THREE.Mesh(geometry, material);";
			myfile << "return mesh;\n";
			myfile << "}\n";
			myfile << "}";
			myfile.close();
			
		}
	}
	/////////////////////////////


	static void saveBoneListMesh(ofstream& myfile, Mesh& mesh) {
	//	if (adata.anim) {
		//	myfile << adata.name << " ";
	//	}
		for (auto& inf : mesh.boneInfo) {
			myfile << inf.name << " ";
			const float* pSource = (const float*)glm::value_ptr(inf.BoneOffset);
			for (unsigned int i = 0; i < 16; i++) {
				myfile << pSource [i]<< " ";
			}
		}

	}


	static void saveAnimation(std::vector<Animation>& animations, string& path) {
		for (Animation& animation : animations) {
			ofstream myfile;
			string finalname = path + "\\" + animation.name + ".anibln";
			std::cout << finalname << "\n";
			myfile.open(finalname);
			myfile << animation.name<<" ";
			myfile << animation.duration << " " << animation.ticksperSec << " " << animation.adata.childAnimationData[0].animationTransformation.size() << "\n";
			myfile << "Actions " << animation.actions.size()<<" ";
			
			for (unsigned int i = 0; i < animation.actions.size(); i++) {
				myfile << animation.actions[i].name << " " << animation.actions[i].range.x << " " << animation.actions[i].range.y << " ";
			}
			myfile << "\n";
			saveBoneAnimation(myfile, animation.adata);
			myfile << "AnimationTree" << "\n";
			//myfile << "hehe";
			saveBoneTree(myfile, animation.adata);

			myfile.close();
			
		}

	}


	static void saveBoneTree(ofstream& myfile, AnimationData& adata) {
		myfile << adata.name;
		if (adata.name != "" && adata.name != "Armature") {
			myfile << " \n";
		}
		
		for (int i = 0; i < adata.childAnimationData.size(); i++) {
			if (adata.name == "") {
				
			}
			else if (adata.name == "Armature") {
				myfile << " ";
			}
			else{
			myfile << adata.name<<" ";
			}
			saveBoneTree(myfile, adata.childAnimationData[i]);
		}
	}
	static void saveBoneAnimation(ofstream& myfile, AnimationData& adata) {
		if (adata.name != "" && adata.name != "Armature") {
			
			myfile << "#\n";
		}
		for (int i = 0; i < adata.animationTransformation.size(); i++) {
			myfile << adata.animationTransformation[i].position.x << " " << adata.animationTransformation[i].position.y << " " << adata.animationTransformation[i].position.z << " ";
			myfile << adata.animationTransformation[i].scale.x << " " << adata.animationTransformation[i].scale.y << " " << adata.animationTransformation[i].scale.z << " ";
			myfile << adata.animationTransformation[i].rotation.w << " " << adata.animationTransformation[i].rotation.x << " " << adata.animationTransformation[i].rotation.y << " " << adata.animationTransformation[i].rotation.z << " ";
			myfile << adata.animationTransformation[i].time;
			myfile << "\n";
		}
		

		
		for (int i = 0; i < adata.childAnimationData.size(); i++) {
			saveBoneAnimation(myfile, adata.childAnimationData[i]);
		}
	}
	static void loadSceneData(string finalPath, std::vector<Primitives> &allPrimitives,std::vector<Animation> &animations, std::vector<Objects> &objects, Lights &sceneLights, Shader &ObjectShader, std::vector<Texture>& textures) {
		string path = "savedData";

		string directory;
		const size_t last_slash_idx = finalPath.rfind('\\');
		if (std::string::npos != last_slash_idx)
		{
			directory = finalPath.substr(0, last_slash_idx);

		}
		string finalname = path + "/saveScene.sav";
		finalname = finalPath;
		ifstream iff(finalname);
		unsigned int NObjects;
		unsigned int NPrimitives;
		string line;
		int state = 0;
		unsigned int primitivesize = allPrimitives.size();
		unsigned int sceneTextureSize = textures.size();
		while (std::getline(iff, line)) {
			
			std::cout << "primitive size " << primitivesize << "\n";
			istringstream iss(line);
			string currentData;
			iss >> currentData;
			if (currentData == "NObjects") {
				string nobjects;
				iss >> nobjects;
				NObjects = std::stoi(nobjects);
				std::cout << "Nobjects " << NObjects << " \n";
			}
			if (currentData == "NPrimitives") {
				string nprimitives;
				iss >> nprimitives;
				NPrimitives = std::stoi(nprimitives);
				std::cout << "NPrimitives " << NPrimitives << " \n";
			}

			if (currentData == "#prim") {
				state = 1;

			}
			else if (currentData == "#animations") {
				state = 2;
			}
			else if (currentData == "#lights") {
				state = 3;

			}
			else if (currentData == "#textures") {
				state = 4;
			}
			
			else if (currentData == "#objects") {
				state = 5;
			}
			
			else if (state == 1) {
				std::cout << "loading primitives\n";
				string finalPathName = directory + "\\primitives" +"\\" + currentData;
				Mesh mesh = LoadPrimitive(finalPathName);
				Primitives prim;
				prim.mesh = mesh;
				prim.name = currentData;
				allPrimitives.push_back(prim);
			}
			else if (state == 2) {
				std::cout << "loading animations \n";
				string finalPathName = directory + "\\animations" + "\\" + currentData;
				Animation animation;
				loadAnimation(finalPathName, animation);
				animations.push_back(animation);
			}

			else if (state == 4) {
				std::cout << "loading textures \n";
				string type = currentData;
				string path;
				if (type == "2D") {
					iss >> path;
					loadTexture(path, textures);

				}
				else if (type == "cubemap") {
					iss >> path;
					loadCubeMapTexture(path, textures);
				}
				//string finalPathName = currentData;
				
			}
			else if (state == 3) {
				std::cout << "loading lights \n";
				string name = currentData;
				int typeIndex;
				glm::vec3 color;
				glm::vec3 pos;
				float strength;
				int index = sceneLights.lights.size();

				iss >> typeIndex;
				Type lightType = static_cast<Type>(typeIndex);
				Profile pro;
				pro.name = name;
				pro.type = lightType;
				for (int j = 0; j < 3; j++) {

					iss >> pos[j];

				}
				for (int i = 0; i < 3; i++) {

					iss >> color[i];

				}
				iss >> strength;

				Light l = Light(pro, pos, color, strength);
				sceneLights.lights.push_back(l);


			}
			else if (state == 5) {
				std::cout << "scene objects";
				string name = currentData;
				glm::vec3 pos;
				glm::vec3 scale;
				glm::vec3 rotation;
				unsigned int primitiveIndex;
				unsigned int animationSize;
				unsigned int animationID;
				unsigned int textureSize;
				std::vector<unsigned int> textureIDS;
				unsigned int materialmodeindex;
				glm::vec4 objectColor = vec4(1.);
		
				iss >> pos.x;
				iss >> pos.y;
				iss >> pos.z;
				iss >> rotation.x;
				iss >> rotation.y;
				iss >> rotation.z;
				iss >> scale.x;
				iss >> scale.y;
				iss >> scale.z;
				iss >> primitiveIndex;
				iss >> animationSize;
				for(unsigned int i=0;i<animationSize;i++){
					iss >> animationID;
				}
				
				primitiveIndex += primitivesize;
				//std::cout << primitiveIndex << "\n";
				//
				iss >> textureSize;
				for (int i = 0; i < textureSize; i++) {
					unsigned int ID;
					iss >> ID;
					ID += sceneTextureSize;
					textureIDS.push_back(ID);
				}
				iss >> materialmodeindex;
				for (int j = 0; j < 4; j++) {
					
					iss >> objectColor[j];
					
				}
			
				Material::Mode m = static_cast<Material::Mode>(materialmodeindex);
				Material def(m);
				iss >> def.roughness;
				iss >> def.metalness;
				iss >> def.ambient;
				iss >> def.blending;
				iss >> def.bloom;
				iss >> def.bloomTexture;
				iss >> def.DiffuseTextureID;
				iss >> def.NormalTextureID;
				iss >> def.GlossyTextureID;
				iss >> def.RoughnessTextureID;
				iss >> def.bloomTextureID;
				iss >> def.brdfTexture;
				iss >> def.environmentTexture;
				iss >> def.irradianceTexture;
				for (auto& object : objects) {

					if (name == object.name) {
						name += "2";
					}
				}
				Objects object(primitiveIndex, name, def);
				object.position = pos;
				object.rotation = rotation;
				object.scale = scale;
				object.textureID = textureIDS;
				object.animationIDs.push_back(animationID);
				objects.push_back(object);

			}

		}
	}

	static void saveSceneData(string filePath, bool exportFully,std::vector<Primitives>& allPrimitives,std::vector<Animation>& animations, std::vector<Objects>& objects, std::vector<Texture>& textures, Lights& sceneLights) {
		//SceneLoader sceneload;

		ofstream myfile;
		string directoryPath = Tools::getPathName(filePath);
		string primitivePath = directoryPath + "\\primitives";
		string texturePath = directoryPath + "\\textures";
		string animationPath = directoryPath + "\\animations";
		CreateDirectory(primitivePath.c_str(), NULL);
		CreateDirectory(texturePath.c_str(), NULL);
		CreateDirectory(animationPath.c_str(), NULL);
		savePrimitive(allPrimitives, primitivePath);
		saveAnimation(animations, animationPath);
		string finalname = filePath;
		myfile.open(finalname);
		myfile << "NObjects " << objects.size() << "\n";
		myfile << "NPrimitives " << allPrimitives.size() << "\n";
		myfile << "NAnimations " << animations.size() << "\n";
		myfile << "NLights " << sceneLights.lights.size() << "\n";
		myfile << "#prim\n";
		for (auto& prim : allPrimitives) {
			myfile << prim.name << "\n";
		}
		myfile << "#animations\n";
		for (Animation& animation : animations) {
			myfile << animation.name << "\n";
		}
		myfile << "#lights\n";
		for (auto& light : sceneLights.lights) {
			myfile << light.profile.name << " ";
			myfile << light.profile.type << " ";
			for (int i = 0; i < 3; i++) {
				myfile << light.position[i] << " ";
			}
			for (int j = 0; j < 3; j++) {
				myfile << light.color[j] << " ";
			}
			myfile << light.strength << "\n";
		}
		myfile << "#textures\n";
		int ID = 0;
		for (auto& tex : textures) {
			if (tex.type == GL_TEXTURE_CUBE_MAP) {
				saveCubeTexture(textures, ID, texturePath);
				tex.path = texturePath +"\\"+tex.name;
			}
			else if (exportFully) {
				string path = texturePath + "\\" + tex.name;
				saveTexture(textures, ID, path);
				tex.path = path;
			}
			
			if (tex.type == GL_TEXTURE_2D) {
				myfile << "2D" << " ";
			}
			else {
				myfile << "cubemap" << " ";
			}
			if (exportFully) {
				myfile << tex.name << "\n";
			}
			else {
				myfile << tex.path << "\n";
			}

			
				
			
			
			ID++;
		}
		myfile << "#objects\n";
		for (auto& ob : objects) {
			myfile << ob.name << " " << ob.position.x << " " << ob.position.y << " " << ob.position.z << " " << ob.rotation.x << " " << ob.rotation.y << " " << ob.rotation.z << " " << ob.scale.x << " " << ob.scale.y << " " << ob.scale.z << " " << ob.primitiveID << " ";
			myfile << ob.animationIDs.size() << " ";
			for (unsigned int animID : ob.animationIDs) {
				myfile << animID << " ";
			}
			myfile << ob.textureID.size()<<" ";
			for (unsigned int texID : ob.textureID) {
				myfile << texID << " ";
			}
			myfile << ob.mat.mode << " ";
			for (int i = 0; i < 4; i++) {
				myfile << ob.mat.objectColor[i] << " ";
			}
			myfile << ob.mat.roughness << " ";
			myfile << ob.mat.metalness << " ";
			myfile << ob.mat.ambient << " ";
			myfile << ob.mat.blending << " ";
			myfile << ob.mat.bloom << " ";
			myfile << ob.mat.bloomTexture << " ";
			myfile << ob.mat.DiffuseTextureID << " ";
			myfile << ob.mat.NormalTextureID << " ";
			myfile << ob.mat.GlossyTextureID << " ";
			myfile << ob.mat.RoughnessTextureID << " ";
			myfile << ob.mat.bloomTextureID << " ";
			myfile << ob.mat.brdfTexture << " ";
			myfile << ob.mat.environmentTexture << " ";
			myfile << ob.mat.irradianceTexture << " ";

			myfile << "\n";

		}
		myfile.close();
	}
	static unsigned int LoadTextureData(const char* path, int &width, int &height,GLenum &format)
	{
		string filename = string(path);


		unsigned int textureID;
		glGenTextures(1, &textureID);
		stbi_set_flip_vertically_on_load(0);
		int nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		float n = 1.0;
		if (data)
		{
			//GLenum format;
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

	static unsigned int LoadCubeMapTextureData(vector<string> faces, int& width, int& height, GLenum& format)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int nrChannels;
		format = GL_RGB;
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
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		return textureID;
	}

	static void loadTexture(string path, std::vector<Texture>& textures) {
		Texture tex;
		int width;
		int height;
		GLenum format = GL_RGBA;
		tex.id = LoadTextureData(path.c_str(),width,height,format);
		tex.type = GL_TEXTURE_2D;
		tex.path = path;
		tex.width = width;
		tex.height = height;
		tex.format = format;
		tex.name = Tools::getFileName(path, true);
		for (auto texture : textures) {
			if (tex.name == texture.name) {
				tex.name += "2";
			}
		}
		textures.push_back(tex);
		//tex2 = tex.id;


	}
	static void setName(Texture &tex,std::vector<Texture>& textures) {
		for (auto texture : textures) {
			if (tex.name == texture.name) {
				tex.name += "2";
			}
		}
	}

	static void loadCubeMapTexture(string path, std::vector<Texture>& textures) {
		Texture tex;
		int width;
		int height;
		GLenum format = GL_RGB;
		vector<string> paths;
		for (int i = 0; i < 6; i++) {
			paths.push_back(path + std::to_string(i) + ".png");
		}
		tex.id = LoadCubeMapTextureData(paths, width, height, format);
		tex.type = GL_TEXTURE_CUBE_MAP;
		tex.path = path;
		tex.width = width;
		tex.height = height;
		tex.format = format;
		tex.name = Tools::getFileName(path, true);
		for (auto texture : textures) {
			if (tex.name == texture.name) {
				tex.name += "2";
			}
		}
		textures.push_back(tex);
		//tex2 = tex.id;


	}
	static void saveCubeTexture(std::vector<Texture> &textures,unsigned int ID, string path) {
		
		
		int numChannels = 0;
		if (textures[ID].format == GL_RED)
			numChannels = 1;
		else if (textures[ID].format == GL_RGB)
			numChannels = 3;
		else if (textures[ID].format == GL_RGBA)
			numChannels = 4;
		const int numberOfPixels = textures[ID].width * textures[ID].height * numChannels;
		 unsigned char* pixels = new unsigned char[numberOfPixels];

		glBindTexture(textures[ID].type, textures[ID].id);
		cout << ID << "\n";
		if (textures[ID].type == GL_TEXTURE_CUBE_MAP) {
			for (int i = 0; i < 6; i++) {
				glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, textures[ID].format, GL_UNSIGNED_BYTE, pixels);
				string finalPath = path + "\\"+textures[ID].name +std::to_string(i)+".png";
			//	stbi_write_bmp(finalPath.c_str(), textures[ID].width, textures[ID].height, numChannels, pixels);
				stbi_write_png(finalPath.c_str(), textures[ID].width, textures[ID].height, numChannels, pixels, numChannels * textures[ID].width);
				
					
				//string finalPath = path + ".bmp";
			}
			
			//stbi_write_bmp(finalPath.c_str(), textures[ID].width, textures[ID].height, numChannels, pixels);
		}
		
	//	glGetTexImage(GL_TEXTURE_2D, 0, textures[ID].format, GL_UNSIGNED_BYTE, pixels);
	//	string finalPath = path + ".bmp";
	//	stbi_write_bmp(finalPath.c_str(), textures[ID].width, textures[ID].height, numChannels, pixels);
	}

	static void saveTexture(std::vector<Texture>& textures,unsigned int ID, string path) {
		string finalPath = path;
		string ext = Tools::getExtensionName(textures[ID].path);

		

		if (Tools::getExtensionName(path) != ext) {
			finalPath += "." + ext;
		}
		//if (Tools::is_file_exist(textures[ID].path.c_str())) {
		if(false){
			CopyFile(textures[ID].path.c_str(), finalPath.c_str(), FALSE);
		}
		else {

			int numChannels = 0;
			if (textures[ID].format == GL_RED)
				numChannels = 1;
			else if (textures[ID].format == GL_RGB)
				numChannels = 3;
			else if (textures[ID].format == GL_RGBA)
				numChannels = 4;
			const int numberOfPixels = textures[ID].width * textures[ID].height * numChannels;
			unsigned char* pixels = new unsigned char[numberOfPixels];
			

			glBindTexture(textures[ID].type, textures[ID].id);

			glGetTexImage(GL_TEXTURE_2D, 0, textures[ID].format, GL_UNSIGNED_BYTE, pixels);
			
			//string ext = Tools::getExtensionName(path);
			std::cout << "extension: " << ext << "\n";
			//int len;
			//unsigned char* png =  stbi_write_png_to_mem((const unsigned char*)pixels, numChannels * textures[ID].width, textures[ID].width, textures[ID].height, numChannels, &len);
			//png[len-1] = 0;
			//std::vector<unsigned int> pixs;
			//std::string st((const char*)png,len);
			//for (unsigned int i = 0; i < len; i++) {
			//	unsigned int k = png[i];
			//	pixs.push_back(k);

			//}
			stbi_write_jpg((finalPath+string(".jpg")).c_str(), textures[ID].width, textures[ID].height, numChannels, pixels, 100);
			stbi_write_png(finalPath.c_str(), textures[ID].width, textures[ID].height, numChannels, pixels, numChannels * textures[ID].width);
		}
	}
};
