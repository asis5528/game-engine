class SceneLoader {
public:

	Mesh LoadPrimitive(string path) {
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
		vector<Texture> tex;
		Mesh m = Mesh(verts, indices, tex);
		return m;

	}
	void savePrimitive(vector<Primitives> &allPrimitives,string &path) {
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

};
