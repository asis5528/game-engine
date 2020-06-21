#include "Events.h"
#include "FBO.h"

struct Primitives
{
	Mesh mesh;
	string name;

};
#include "SceneLoader.h"
class Scene {
public:
	Camera *camera;
	std::vector<Objects> objects;
	std::vector<Primitives> allPrimitives;
	std::vector<int> selected;
	int selectionIndex = 0;
	int previous = -1;
	Shader ObjectShader;
	Shader pickingShader;
	Quad q;
	LineRenderer line;
	LineRenderer line2;
	LineRenderer line3;
	Lights sceneLights;
	FBO fbo;
	int width;
	int height;
	GLFWwindow* window;
	glm::vec3 ClearColor;
	
	int tex2;


	bool firstAction = true;
	glm::vec3 currenRotation = glm::vec3(0., 0., 0.);

	Scene(Camera &cam) {
		ObjectShader = Shader("Data/object.vert", "Data/object.frag");
		pickingShader = Shader("Data/picking.vert", "Data/picking.frag");
		q.init();
		line =  LineRenderer(vec3(-10, 0, -10), vec3(10, 0, -10));
		line2 = LineRenderer(vec3(-10, 0, 10), vec3(-10, 0, -10));
		line3 = LineRenderer(vec3(0, -1000, 0), vec3(0, 1000, 0));
		tex2 = q.LoadTexture("Data/logo.png");
		q.tex.id = tex2;
		camera = &cam;
		glEnable(GL_DEPTH_TEST);
		fbo = FBO(camera->width, camera->height, 2);
	}
	Scene() {

	}
	
	void draw(Camera &camera) {
		fbo.bind();
		glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, 1.);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		line.draw(camera, vec3(0.0, 0.0, 1.),21);
		line2.draw(camera, vec3(1.0, 0.0, 0.),21);
		glLineWidth(1.0);
		if (action_axis == 2) {
			
			line3.draw(camera, vec3(0, 0, 0), 1, vec3(0., 0., 1.), objects[selectionIndex].position);
		}
		else if (action_axis == 1) {
			
			glm::mat4 model(1.0);
			glm::translate(model, objects[selectionIndex].position);
			glm::rotate(model, 1.5708f, glm::vec3(0, 0, 1));
			line3.draw(camera, vec3(0, 0, 0), 1, vec3(1., 0., 0.), objects[selectionIndex].position, glm::vec4(0, 0, 1, 1.5708f));
		}
		else if (action_axis == 3) {
			
			glm::mat4 model(1.0);
			line3.draw(camera, vec3(0, 0, 0), 1, vec3(0., 1., 0.), objects[selectionIndex].position, glm::vec4(1, 0, 0, 1.5708f));
		}
	
		glLineWidth(1.0);
		
	
		for (Objects &object : objects) {

			object.draw(camera,allPrimitives[object.primitiveID].mesh);
		}
		glViewport(0, 0, width, height);
		fbo.ubind();
		glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, 1.);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (1) {
			q.textures.push_back(  fbo.textures[0]);
			q.textures.push_back(fbo.textures[1]);
			q.textures.push_back(tex2);
			q.draw(glfwGetTime());
		}
	}

	std::string getFileName(std::string filePath, bool withExtension = true, char seperator = '\\')
	{
		// Get last dot position
		std::size_t dotPos = filePath.rfind('.');
		std::size_t sepPos = filePath.rfind(seperator);

		if (sepPos != std::string::npos)
		{
			return filePath.substr(sepPos + 1, filePath.size() - (withExtension || dotPos != std::string::npos ? 1 : dotPos));
		}
		return "";
	}

	void LoadObject(int &path_count, const char* paths[]) {
		for (int i = 0; i < path_count; i++) {
			const char* path = paths[i];
			string fileName = getFileName(path, false);
			const char* name = fileName.c_str();
			Model model(path);
			ofstream myfile;
			for (int i = 0; i < model.meshes.size();i++) {
				Objects object(allPrimitives.size(), ObjectShader, model.names[i],sceneLights);
				Primitives p;
				p.mesh = model.meshes[i];
				p.name = model.names[i];
				allPrimitives.push_back(p);
				object.position = model.positions[i];
				object.scale = model.scales[i];
				object.rotation = model.rotations[i];
				objects.push_back(object);
			}
			
		}

	}
	void CustomLoad(Mesh &mesh,string name) {
		//need to update
		Objects object(0, ObjectShader, name, sceneLights);
		objects.push_back(object);
	}
	void loadScene(string& path) {
		SceneLoader sceneload  = SceneLoader();
		Mesh mesh = sceneload.LoadPrimitive(path);
		CustomLoad(mesh,"name");
	}
	

	void saveSceneData(string directoryPath) {
		SceneLoader sceneload;
		
		ofstream myfile;
	
		sceneload.savePrimitive(allPrimitives, directoryPath);
		string finalname = directoryPath +"\\saveScene.sav";
		myfile.open(finalname);
		myfile << "NObjects " << objects.size() << "\n";
		myfile << "NPrimitives " << allPrimitives.size() << "\n";
		myfile << "#prim\n";
		for (auto& prim : allPrimitives) {
			myfile << prim.name << "\n";
		}
		myfile << "#objects\n";
		for (auto& ob : objects) {
			myfile << ob.name<<" " << ob.position.x << " " << ob.position.y << " " << ob.position.z << " " << ob.rotation.x << " " << ob.rotation.y << " " << ob.rotation.z << " " << ob.scale.x << " " << ob.scale.y << " " << ob.scale.z << " " << ob.primitiveID << "\n";

		}
		myfile.close();
	}

	void loadSceneData(string finalPath) {
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
		while (std::getline(iff, line)) {
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
			else if (currentData == "#objects") {
				state = 2;
			}
			else if (state == 1) {
				//std::cout << currentData << "\n";
				SceneLoader sceneload = SceneLoader();
				std::cout << directory+"\\"+currentData << "\n";
				string finalPathName = directory + "\\" + currentData;
				Mesh mesh = sceneload.LoadPrimitive(finalPathName);
				Primitives prim;
				prim.mesh = mesh;
				prim.name = currentData;
				allPrimitives.push_back(prim);

			}
			else if (state == 2) {
				string name = currentData;
				glm::vec3 pos;
				glm::vec3 scale;
				glm::vec3 rotation;
				unsigned int primitiveIndex;
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
				std::cout << primitiveIndex << "\n";
				Objects object(primitiveIndex, ObjectShader,name, sceneLights);
				object.position = pos;
				object.rotation = rotation;
				object.scale = scale;
				objects.push_back(object);

			}
		
		}
	}

	void pickObject() {
		glClearColor(2.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pickingShader.use();
		int i = 0;
		for (Objects object : objects) {
			float col = i / 255.0f;
			pickingShader.setFloat("color", col);
			object.drawCustom(*camera, pickingShader, allPrimitives[object.primitiveID].mesh);
			i += 1;
		}
		glUseProgram(0);
		glFlush();
		glFinish();
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		unsigned char data[4];
		glReadPixels(xpos, height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top
		//Convert the color back to an integer ID
		int gPickedIndex = int(data[0]);
		cout << gPickedIndex << "\n";
		//cout << gPickedIndex << "\n";
		if (objects.size() > 0 && gPickedIndex != 255) {
			if (previous != gPickedIndex && previous > -1) {
				objects[previous].selected = false;
			}
			objects[gPickedIndex].selected = true;
			selectionIndex = gPickedIndex;
			previous = gPickedIndex;
			cout <<objects[gPickedIndex].name << "\n";
		}
	}

	void DuplicateObject() {
		if (objects.size() > 0) {
			string finalName = objects[selectionIndex].name + std::to_string(objects.size());
			Objects dup = Objects(objects[selectionIndex].primitiveID, objects[selectionIndex].shader, finalName,sceneLights);
			dup.position = objects[selectionIndex].position;
			dup.rotation = objects[selectionIndex].rotation;
			objects.push_back(dup);
		}
	}

	

};