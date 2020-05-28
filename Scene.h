#include "Events.h"
#include "FBO.h"

class Scene {
public:
	Camera *camera;
	std::vector<Objects> objects;
	std::vector<int> selected;
	int selectionIndex = 0;
	int previous = -1;
	Shader ObjectShader;
	Shader pickingShader;
	Quad q;
	LineRenderer line;
	LineRenderer line2;
	LineRenderer line3;

	FBO fbo;

	int width;
	int height;
	GLFWwindow* window;
	glm::vec3 ClearColor;
	Objects projection;
	int tex2;

	float n = 0.;
	bool firstAction = true;
	glm::vec3 currenRotation = glm::vec3(0., 0., 0.);


	float zoom = 30.;
	float max = 0.;
	glm::vec3 pano2 = glm::vec3(0,1.4,0.41);
	std::vector<glm::vec3> checkpoints{ vec3(0.0f, 1.4f, 4.1859f) ,vec3(0.0f, 1.4f, 2.5225f),vec3(0.0f, 1.4f, 0.41f),vec3(1.9761f, 1.4f, -1.2944f) };



	Scene(Camera &cam) {
		ObjectShader = Shader("Data/object.vert", "Data/object.frag");
		pickingShader = Shader("Data/picking.vert", "Data/picking.frag");
		q.init();
		line = LineRenderer(vec3(-10, 0, -10), vec3(10, 0, -10));
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

			object.draw(camera);
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
			for (int i = 0; i < model.meshes.size();i++) {
				Objects object(model.meshes[i], ObjectShader, model.names[i]);
			//	object.modelMatrix = model.transformations[i];
				object.position = model.positions[i];
				object.scale = model.scales[i];
				object.rotation = model.rotations[i];
				//object.selected = true;
				objects.push_back(object);
			}

		}

	}

	void pickObject() {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pickingShader.use();
		int i = 0;
		for (Objects object : objects) {
			float col = i / 255.0f;
			pickingShader.setFloat("color", col);
			object.drawCustom(*camera, pickingShader);
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
			Objects dup = Objects(objects[selectionIndex].mesh, objects[selectionIndex].shader, objects[selectionIndex].name + std::to_string(objects.size()));
			dup.position = objects[selectionIndex].position;
			dup.rotation = objects[selectionIndex].rotation;
			objects.push_back(dup);
		}
	}
	

};