#include "Events.h"
#include "FBO.h"
#include "Cube.h"
#include "CubeMapGenerator.h"
#include "IrradianceMapGenerator.h"
#include "Tools.h"
struct Primitives
{
	Mesh mesh;
	string name;

};
struct Texture {
	string name;
	unsigned int id;
	GLenum type = GL_TEXTURE_2D;
	unsigned int width;
	unsigned int height;
	GLenum format = GL_RGBA;
	string path;
};
#include "Material.h"
#include "Objects.h"
#include "SceneLoader.h"
class Scene {
public:
	Camera *camera;
	std::vector<Objects> objects;
	std::vector<Primitives> allPrimitives;
	std::vector<Texture> textures;
	std::vector<int> selected;
	int selectionIndex = 0;
	int previous = -1;
	Shader ObjectShader;
	Shader DefaultShader;
	Shader TexturedShader;
	Shader pickingShader;
	Shader bloom1Shader;
	Shader bloom2Shader;
	Quad q;
	Cube cube;
	LineRenderer line;
	LineRenderer line2;
	LineRenderer line3;
	Lights sceneLights;
	FBO fbo;
	FBO fbo2;
	FBO bloom1;
	FBO bloom2;
	FBO bloom3;
	FBO bloom4;
	bool onlyRender = false;
	//Material ml
	int width;
	int height;
	GLFWwindow* window;
	glm::vec4 ClearColor = glm::vec4(0.);
	int tex2;
	bool firstAction = true;
	glm::vec3 currenRotation = glm::vec3(0., 0., 0.);
	std::vector<Shader> shads;
	//Material m;
	Shader skyshader;
	CubeMapGenerator mapgen;


	
	Scene(Camera &cam) {
		ObjectShader = Shader("Data/Shaders/object.vert", "Data/Shaders/object.frag");
		DefaultShader = Shader("Data/Shaders/Default.vert", "Data/Shaders/Default.frag");
		TexturedShader  = Shader("Data/Shaders/Textured.vert", "Data/Shaders/Textured.frag");
		Shader pbr = Shader("Data/Shaders/pbr.vert", "Data/Shaders/pbr.frag");
		skyshader = Shader("Data/Shaders/skybox.vert", "Data/Shaders/skybox.frag");
		shads.push_back(DefaultShader);
		shads.push_back(TexturedShader);
		shads.push_back(pbr);//setting this to index 3...coz light is replaced by this
		//m  = Material();
		mapgen.init();
		cube.init();
	
		pickingShader = Shader("Data/Shaders/picking.vert", "Data/Shaders/picking.frag");
		bloom1Shader = Shader("Data/Shaders/Hblur.vert", "Data/Shaders/bloom1.frag");
		bloom2Shader = Shader("Data/Shaders/Vblur.vert", "Data/Shaders/bloom1.frag");
		q.init();
		line =  LineRenderer(vec3(-10, 0, -10), vec3(10, 0, -10));
		line2 = LineRenderer(vec3(-10, 0, 10), vec3(-10, 0, -10));
		line3 = LineRenderer(vec3(0, -1000, 0), vec3(0, 1000, 0));
		tex2 = q.LoadTexture("Data/logo.png");
		//q.tex.id = tex2;
		camera = &cam;
		//glDisable(GL_DEPTH_TEST);
		fbo = FBO(1980, 1024, 2);
		fbo2 = FBO(camera->width, camera->height, 1);
		bloom1 = FBO(camera->width, camera->height, 1);
		bloom2 = FBO(camera->width, camera->height, 1);
		bloom3 = FBO(camera->width , camera->height, 1);
		bloom4 = FBO(camera->width, camera->height , 1);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		
		
	}
	Scene() {

	}
	
	void drawObjects(bool blend) {
		
		for (Objects& object : objects) {
			if (object.mat.blending == blend) {
			
				object.mat.set(shads, sceneLights, textures, object.textureID,camera);
				/*
				object.shader.use();
				if (object.textureID.size() > 0) {
					object.shader.setInt("useTexture", 1);
					int index = object.textureID[0];
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, textures[index].id);

				}
				else {
					object.shader.setInt("useTexture", 0);
				}
				object.shader.setInt("selected", 0);
				*/
				object.mat.start();
				object.mat.shad->setInt("selected", 0);
				object.draw(*camera, allPrimitives[object.primitiveID].mesh);
			}
		}
	}
	void environmentMapGenerator(unsigned int ID ) {
		unsigned int tex = mapgen.generateCubeMap(512, 512, textures[ID].id);
		Texture t;
		t.id = tex;
		t.name = "environmentTex";
		SceneLoader::setName(t, textures);
		t.type = GL_TEXTURE_CUBE_MAP;
		t.width = 512;
		t.height = 512;
		t.format = GL_RGB;
		textures.push_back(t);

		unsigned int tex1 = mapgen.generateIrradianceMap();
		Texture t1;
		t1.id = tex1;
		t1.name = "irradianceTex";
		SceneLoader::setName(t1, textures);
		t1.type = GL_TEXTURE_CUBE_MAP;
		t1.width = 32;
		t1.height = 32;
		t1.format = GL_RGB;
		textures.push_back(t1);

		unsigned int tex2 = mapgen.prefilterMapGeneration();
		Texture t2;
		t2.id = tex2;
		t2.name = "prefilterTex";
		SceneLoader::setName(t2, textures);
		t2.type = GL_TEXTURE_CUBE_MAP;
		t2.width = 128;
		t2.height = 128;
		t2.format = GL_RGB;
		textures.push_back(t2);

	}


	void save() {

		Mesh m = allPrimitives[objects[selectionIndex].primitiveID].mesh;
		ofstream myfile;
		myfile.open("saved.txt");
		for (int i = 0; i < m.vertices.size();i++) {
			myfile <<"glVertex3d(" <<m.vertices[i].Position.x <<" ,"<< m.vertices[i].Position.y << ", " << m.vertices[i].Position.z<<");"<<"\n";

		}
	}
	void draw(Camera &camera) {
		/*
		if (textures.size() > 0) {
			if (!mapgen.state) {

			
			unsigned int tex = mapgen.generateCubeMap(512, 512, textures[0].id);
			Texture t;
			t.id = tex;
			t.name = "environmentTex";
			textures.push_back(t);

			unsigned int tex1 = mapgen.generateIrradianceMap();
			Texture t1;
			t1.id = tex1;
			t1.name = "irradianceTex";
			textures.push_back(t1);

			unsigned int tex2 = mapgen.prefilterMapGeneration();
			Texture t2;
			t2.id = tex2;
			t2.name = "prefilterTex";
			textures.push_back(t2);
			}
		}
		*/
		fbo.bind();
		glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (!onlyRender) {


		glLineWidth(2.0f);
		line.draw(camera, vec3(0.0, 0.0, 1.),21);
		line2.draw(camera, vec3(1.0, 0.0, 0.),21);
		
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
	
		glLineWidth(2.0);
		}

		skyshader.use();
		skyshader.setMat4("projection", camera.projMat);
		skyshader.setMat4("view", camera.GetViewMatrix());
		if (textures.size() > 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,textures[0].id);
			skyshader.setInt("equirectangularMap", 0);
		}
		//cube.draw(skyshader);

		drawObjects(false);
		drawObjects(true);


		
		glViewport(0, 0, width, height);
	
		fbo.ubind();





		

			fbo2.bind();
			glClearColor(0., 0., 0., 1.);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			if (!onlyRender) {
			for (Objects& object : objects) {
				if (object.name == objects[selectionIndex].name) {
					object.mat.shad->use();
					object.mat.shad->setInt("selected", 1);
					object.draw(camera, allPrimitives[object.primitiveID].mesh);
				}
			}
			}
			glViewport(0, 0, width, height);
			fbo2.ubind();
			bool bloom = false;
			if (bloom) {




		bloom1.bind();
		q.textures.push_back(fbo.textures[1]);
		bloom1Shader.use();
		bloom1Shader.setFloat("width", 2000 /8);
		q.drawCustom(bloom1Shader.ID);
		q.textures.clear();
		bloom1.ubind();


		bloom2.bind();
	//	glClearColor(0., 0., 0., 1.);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		q.textures.push_back(bloom1.textures[0]);
		bloom2Shader.use();
		bloom2Shader.setFloat("height", 2000 / 8);
		q.drawCustom(bloom2Shader.ID);
		q.textures.clear();
		bloom2.ubind();
		
		bloom3.bind();
	//	glClearColor(0., 0., 0., 1.);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		q.textures.push_back(bloom2.textures[0]);
		bloom1Shader.use();
		bloom1Shader.setFloat("width", 2000 / 4);
		q.drawCustom(bloom1Shader.ID);
		q.textures.clear();
		bloom3.ubind();

		bloom4.bind();
	//	glClearColor(0., 0., 0., 1.);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		q.textures.push_back(bloom3.textures[0]);
		bloom1Shader.use();
		bloom1Shader.setFloat("width", 2000 / 4);
		q.drawCustom(bloom1Shader.ID);
		q.textures.clear();
		bloom4.ubind();
			}

		if (textures.size() > 0) {
		//	fbo.textures[0] = textures[0].id;
		}

		
		glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);
		if (1) {
			q.textures.push_back(  fbo.textures[0]);
			q.textures.push_back(fbo2.textures[0]);
			q.textures.push_back(bloom4.textures[0]);
			glUseProgram(q.shaderProgram);
			glUniform1f(glGetUniformLocation(q.shaderProgram, "width"), width);
			glUniform1f(glGetUniformLocation(q.shaderProgram, "height"), height);
			glUniform1i(glGetUniformLocation(q.shaderProgram, "bloom"), bloom);
			q.draw(glfwGetTime());
			q.textures.clear();
		}
	}

	
	void LoadObject(int &path_count, const char* paths[]) {
		for (int i = 0; i < path_count; i++) {
			const char* path = paths[i];
			string fileName = Tools::getFileName(path, false);
			string exName = Tools::getExtensionName(path);
			if (exName == "FBX" || exName == "fbx" || exName == "obj" || exName == "OBJ" || exName == "DAE" || exName == "dae")
			{
				const char* name = fileName.c_str();
				
				Model model(path);
				ofstream myfile;
				Material::Mode m = Material::Default;
				Material def(m);
				for (int i = 0; i < model.meshes.size(); i++) {
					for (auto& object : objects) {
			
						if (model.names[i] == object.name) {
							model.names[i] += "2";
						}
					}
					model.names[i] = Tools::space2underscore(model.names[i]);
					Objects object(allPrimitives.size(), model.names[i], def);
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
			else if (exName == "jpg" || exName == "JPG" || exName == "png" || exName == "PNG" || 
				exName == "jpeg" || exName == "JPEG" || exName == "tga" || exName == "TGA" || exName == "hdr" ||
				exName == "HDR" || exName == "bmp" || exName == "BMP") 
			{
				loadTexture(path);
			}
			else if (exName == "sav") {
				loadSceneData(path);
			}
			
		}

	}
	void CustomLoad(Mesh &mesh,string name) {
		//need to update
		Material::Mode m = Material::Default;
		Material def(m);
		Objects object(0, name,def);
		objects.push_back(object);
	}
	void loadScene(string& path) {
		SceneLoader sceneload = SceneLoader();
		Mesh mesh = sceneload.LoadPrimitive(path);
		CustomLoad(mesh,"name");
	}
	
	void saveTexture(unsigned int ID,string path) {
		SceneLoader::saveTexture(textures, ID, path);
	}

	void saveSceneData(string directoryPath,bool exportFully = false) {
	
		SceneLoader::saveSceneData(directoryPath, exportFully,allPrimitives, objects,textures,sceneLights);
	}

	void loadSceneData(string finalPath) {
		SceneLoader::loadSceneData(finalPath,allPrimitives,objects,sceneLights,ObjectShader ,textures);
	}

	void loadTexture(string path) {
		SceneLoader::loadTexture(path, textures);

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
			Objects dup = Objects(objects[selectionIndex].primitiveID, finalName, objects[selectionIndex].mat);
			dup.position = objects[selectionIndex].position;
			dup.rotation = objects[selectionIndex].rotation;
			dup.scale = objects[selectionIndex].scale;
			objects.push_back(dup);
		}
	}
	void deleteBuffers() {
		for (auto& tex : textures) {
			glDeleteTextures(1, &tex.id);
		}
	}
	

};