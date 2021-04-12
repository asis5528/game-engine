#include "AnimationData.h"
#include "Animation.h"
#include "model.h"
#include "Events.h"
#include "FBO.h"
#include "Cube.h"
#include "CubeMapGenerator.h"
#include "IrradianceMapGenerator.h"
#include "Tools.h"
struct Primitives
{
	bool change = false;
	bool selected = false;
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
	void dispose() {
		glDeleteTextures(1, &id);
	}
};
#include "Material.h"
#include "Objects.h"
#include "SceneLoader.h"
class Scene {
public:
	Camera* camera;
	std::vector<Objects> objects;
	std::vector<Primitives> allPrimitives;
	std::vector<AnimationData> adata;
	std::vector<Texture> textures;
	std::vector<Animation> animations;
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
	FBO depth;
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
	Model model;
	Camera depthCam = Camera(glm::vec3(0.0, 0.0, 0.0));
	glm::mat4 oldMat = glm::mat4(1.0);
	Scene(Camera& cam) {
		ObjectShader = Shader("Data/Shaders/object.vert", "Data/Shaders/object.frag");
		DefaultShader = Shader("Data/Shaders/Default.vert", "Data/Shaders/Default.frag");
		//DefaultShader = Shader("Data/Shaders/Default.vert", "Data/Shaders/RayMarching.frag");
		//TexturedShader = Shader("Data/Shaders/Default.vert", "Data/Shaders/Refraction.frag");
		TexturedShader = Shader("Data/Shaders/Default.vert", "Data/Shaders/Textured.frag");
		Shader pbr = Shader("Data/Shaders/Default.vert", "Data/Shaders/pbr.frag");
		skyshader = Shader("Data/Shaders/skybox.vert", "Data/Shaders/skybox.frag");
		shads.push_back(DefaultShader);
		shads.push_back(TexturedShader);
		shads.push_back(pbr);//setting this to index 3...coz light is replaced by this
		//m  = Material();
		mapgen.init();
		cube.init();
		depthCam.setProjectionMatrix(512, 512);
		depthCam.fov = 90;
		pickingShader = Shader("Data/Shaders/default.vert", "Data/Shaders/picking.frag");
		bloom1Shader = Shader("Data/Shaders/Hblur.vert", "Data/Shaders/bloom1.frag");
		bloom2Shader = Shader("Data/Shaders/Vblur.vert", "Data/Shaders/bloom1.frag");
		q.init();
		line = LineRenderer(vec3(-10, 0, -10), vec3(10, 0, -10));
		line2 = LineRenderer(vec3(-10, 0, 10), vec3(-10, 0, -10));
		line3 = LineRenderer(vec3(0, -1000, 0), vec3(0, 1000, 0));
		tex2 = q.LoadTexture("Data/logo.png");
		//q.tex.id = tex2;
		camera = &cam;
		//glDisable(GL_DEPTH_TEST);
		fbo = FBO(1980, 1024, 2);
		depth = FBO(1024, 1024, 1,true);
		fbo2 = FBO(camera->width, camera->height, 1);
		bloom1 = FBO(camera->width, camera->height, 1);
		bloom2 = FBO(camera->width, camera->height, 1);
		bloom3 = FBO(camera->width, camera->height, 1);
		bloom4 = FBO(camera->width, camera->height, 1);
	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		/*
		const char* k[3] = { "D:\\3d_Models\\cam2.dae","D:\\3d_Models\\quad.dae","D:\\3d_Models\\suzz.dae" };
		int n = 3;
		LoadObject(n, k);

		
				Texture t;
				t.id = depth.textures[0];
				textures.push_back(t);
		
		
		objects[0].mat.objectColor.r = 0.9;
		objects[1].textureID.push_back(0);
		objects[1].mat.mode = static_cast<Material::Mode>( 1);
		*/
	}
	Scene() {

	}

	void drawObjects(bool blend,Camera &cam) {
		int i = 0;
		static bool parent = false;
		static float frame = 0.0;
		for (Objects& object : objects) {
			if (object.mat.blending == blend) {

				object.mat.set(shads, sceneLights, textures, object.textureID, camera);

				object.mat.start();
				object.mat.shad->setInt("selected", 0);


				object.sendTransforms(cam, allPrimitives[object.primitiveID].mesh);
				if(true){
				if(i<1){
						if(frame>1){
							if (!parent) {
								glm::mat4 matt = glm::inverse(objects[3].modelMatrix);
								glm::mat4 tes(1.0);
								glm::scale(tes,objects[3].scale);
								oldMat = matt;
								parent = true;
							}
						}
						glm::mat4 fin =  objects[3].modelMatrix * oldMat;
						//objects[0].modelMatrix = objects[3].modelMatrix * objects[0].modelMatrix;
						objects[0].modelMatrix = fin * objects[0].modelMatrix;
					}
					i += 1;
				}
				
				if (object.animationIDs.size() > 0) {
					object.processAnimation(allPrimitives[object.primitiveID].mesh, animations[object.animationIDs[0]]);
				}
				object.draw(cam, allPrimitives[object.primitiveID].mesh);
			}
		}
		if (objects.size() > 0) {
			frame += 1;
		}
		
	}
	void environmentMapGenerator(unsigned int ID) {
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


	


	void RenderParts(Camera &camera) {
		glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (!onlyRender) {


			glLineWidth(2.0f);
			line.draw(camera, vec3(0.0, 0.0, 1.), 21);
			line2.draw(camera, vec3(1.0, 0.0, 0.), 21);

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

		//skyshader.use();
		//skyshader.setMat4("projection", camera.projMat);
		//skyshader.setMat4("view", camera.GetViewMatrix());
		//if (textures.size() > 0) {
		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, textures[0].id);
		//	skyshader.setInt("equirectangularMap", 0);
		//}
		//cube.draw(skyshader);

		drawObjects(false, camera);
		drawObjects(true, camera);
		static float k = 0;
		if (objects.size() > 4) {
			k += 0.1;
			for (unsigned int i = 0; i < objects[0].boneMatrices.size(); i++) {
				glm::mat4 sc(1.0);
				
				sc = glm::scale(sc, glm::vec3(0.1));
				glm::mat4 sc1(1.0);
				sc1 = glm::scale(sc1, objects[0].scale);
				
				objects[4].modelMatrix = sc1 *objects[0].boneMatrices[i]*sc;
		
				objects[4].draw(camera, allPrimitives[objects[4].primitiveID].mesh);
			}
		}
		


		glViewport(0, 0, width, height);
	}

	void draw(Camera& camera) {
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
		depth.bind();
		glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (unsigned int i = 1; i < objects.size();i++) {
			

				objects[i].mat.set(shads, sceneLights, textures, objects[i].textureID, &depthCam);
				objects[i].mat.start();
				objects[i].mat.shad->setInt("selected", 0);
				objects[i].drawDepth( allPrimitives[objects[i].primitiveID].mesh);
			
		}

		depth.ubind();


		
		fbo.bind();
		RenderParts(camera);

		fbo.ubind();







		fbo2.bind();
		glClearColor(0., 0., 0., 1.);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (!onlyRender) {
			
			for (Objects& object : objects) {
				if (object.name == objects[selectionIndex].name) {
					object.mat.shad->use();
					//object.sendTransforms(camera, allPrimitives[object.primitiveID].mesh);
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
			bloom1Shader.setFloat("width", 2000 / 8);
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
			q.textures.push_back(fbo.textures[0]);
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


	void LoadObject(int& path_count, const char* paths[]) {
		for (int i = 0; i < path_count; i++) {
			const char* path = paths[i];
			std::cout << paths[i] << "\n";
			string fileName = Tools::getFileName(path, false);
			string exName = Tools::getExtensionName(path);
			if (exName == "FBX" || exName == "fbx" || exName == "obj" || exName == "OBJ" || exName == "DAE" || exName == "dae")
			{
				const char* name = fileName.c_str();

				//Model model(path);
				model = Model(path);

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
					model.sceneAnimation.name = "animation" + std::to_string(animations.size());


					Primitives p;
					p.mesh = model.meshes[i];
					p.name = model.names[i];
					if (p.mesh.hasBones) {
						animations.push_back(model.sceneAnimation);
						object.animationIDs.push_back(animations.size() - 1);
					}

					allPrimitives.push_back(p);
					object.position = model.positions[i];
					object.scale = model.scales[i];
					object.rotation = model.rotations[i];
					object.dimension = allPrimitives[object.primitiveID].mesh.Dimensions * object.scale;
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
	void CustomLoad(Mesh& mesh, string name) {
		//need to update
		Material::Mode m = Material::Default;
		Material def(m);
		Objects object(0, name, def);
		objects.push_back(object);
	}
	void loadScene(string& path) {
		SceneLoader sceneload = SceneLoader();
		Mesh mesh = sceneload.LoadPrimitive(path);
		CustomLoad(mesh, "name");
	}

	void saveTexture(unsigned int ID, string path) {
		SceneLoader::saveTexture(textures, ID, path);
	}

	void saveSceneData(string directoryPath, bool exportFully = false) {

		SceneLoader::saveSceneData(directoryPath, exportFully, allPrimitives, animations, objects, textures, sceneLights);
	}
	void saveJsScene(string directoryPath) {
		SceneLoader::savejsPrimitive(objects,allPrimitives, directoryPath,textures);
	}

	void loadSceneData(string finalPath) {
		SceneLoader::loadSceneData(finalPath,allPrimitives,animations,objects,sceneLights,ObjectShader ,textures);
	}

	void loadTexture(string path) {
		SceneLoader::loadTexture(path, textures);

	}
	void reset() {
		//objects.clear();
		for (unsigned int i = 0; i < textures.size(); i++) {
			textures[i].dispose();
		}
		textures.clear();
		for (unsigned int i = 0; i < allPrimitives.size(); i++) {
			allPrimitives[i].mesh.dispose();
		}
	
			adata.clear();
		
		
			animations.clear();
			objects.clear();
		
		
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
			Objects dup = objects[selectionIndex];
			dup.name = finalName;
		
			objects.push_back(dup);
		}
	}
	void deleteBuffers() {
		for (auto& tex : textures) {
			glDeleteTextures(1, &tex.id);
		}
	}
	

};