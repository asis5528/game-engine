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
		string line;
		int state = 0;
		while (std::getline(iff, line)) {
			string n;
			iff >> n;
		
			if (state == 1) {

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
			if (n == "vertexDatas") {
				state = 1;
				
			}
		}
		vector<Vertex> verts;
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

		float k = 1.;
		Mesh m = Mesh(verts, indices);
		return m;

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
			for (auto& v : p.mesh.vertices) {
				myfile << v.Position.x << " " << v.Position.y << " " << v.Position.z << " ";
				myfile << v.Normal.x << " " << v.Normal.y << " " << v.Normal.z << " ";
				myfile << v.TexCoords.x << " " << v.TexCoords.y << " ";
			}
			myfile << " \n";
			myfile << "indices \n";
			for (auto i : p.mesh.indices) {
				myfile << i << " ";
			}
			myfile.close();
		}
	}
	static void loadSceneData(string finalPath, std::vector<Primitives> &allPrimitives, std::vector<Objects> &objects, Lights &sceneLights, Shader &ObjectShader, std::vector<Texture>& textures) {
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
			else if (currentData == "#textures") {
				state = 2;
			}
			else if (currentData == "#lights") {
				state = 4;

			}
			else if (currentData == "#objects") {
				state = 3;
			}
			else if (state == 1) {
				std::cout << directory + "\\" + currentData << "\n";
				string finalPathName = directory + "\\primitives" +"\\" + currentData;
				Mesh mesh = LoadPrimitive(finalPathName);
				Primitives prim;
				prim.mesh = mesh;
				prim.name = currentData;
				allPrimitives.push_back(prim);
			}
			else if (state == 2) {
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
			else if (state == 4) {
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
			else if (state == 3) {
				string name = currentData;
				glm::vec3 pos;
				glm::vec3 scale;
				glm::vec3 rotation;
				unsigned int primitiveIndex;
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
				primitiveIndex += primitivesize;
				std::cout << primitiveIndex << "\n";
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
				//object.mat.mode = static_cast<Material::Mode>( 0);
				objects.push_back(object);

			}

		}
	}

	static void saveSceneData(string filePath, bool exportFully,std::vector<Primitives>& allPrimitives, std::vector<Objects>& objects, std::vector<Texture>& textures, Lights& sceneLights) {
		//SceneLoader sceneload;

		ofstream myfile;
		string directoryPath = Tools::getPathName(filePath);
		string primitivePath = directoryPath + "\\primitives";
		string texturePath = directoryPath + "\\textures";
		CreateDirectory(primitivePath.c_str(), NULL);
		CreateDirectory(texturePath.c_str(), NULL);
		savePrimitive(allPrimitives, primitivePath);
		string finalname = filePath;
		myfile.open(finalname);
		myfile << "NObjects " << objects.size() << "\n";
		myfile << "NPrimitives " << allPrimitives.size() << "\n";
		myfile << "NLights " << sceneLights.lights.size() << "\n";
		myfile << "#prim\n";
		for (auto& prim : allPrimitives) {
			myfile << prim.name << "\n";
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
		if (Tools::is_file_exist(textures[ID].path.c_str())) {
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

			stbi_write_png(finalPath.c_str(), textures[ID].width, textures[ID].height, numChannels, pixels, numChannels * textures[ID].width);
		}
	}
};
