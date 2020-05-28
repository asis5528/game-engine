#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_m.h"
#include "camera.h"
#include "model.h"
#include "quad.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// camera
static Camera camera(glm::vec3(0.0f, 7.0f, 25.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


vector<Model> models;
//constant position of head
glm::vec3 pos(0.0, 4.78732, 0.0f);

//local position of head
glm::vec3 headPos(0.0, 6.946, 0.0f);
//local position of body
glm::vec3 bodyPos(0.0, 4.78732 - 1.0, 0.0f);
//local position of left leg
glm::vec3 LlegPos(0.0, 3.35326, -0.29189f);
//local position of right leg
glm::vec3 RlegPos(0.0, 3.35326, 0.34412f);
//local position of left hand
glm::vec3 LhandPos(0.0, 5.57974, -0.81956f);
//local position of right hand
glm::vec3 RhandPos(0.0, 5.57974, 0.85876f);

//head transformation matrix initialization
glm::mat4 headMat(1.0);
//body transformation matrix initialization
glm::mat4 bodyMat(1.0);
//left leg  transformation matrix initialization
glm::mat4 LlegMat(1.0);
//right leg transformation matrix initialization
glm::mat4 RlegMat(1.0);
//left handtransformation matrix initialization
glm::mat4 LhandMat(1.0);
//rigth hand transformation matrix initialization
glm::mat4 RhandMat(1.0);

Quad q;


// timing
float deltaTime = 0.0f;
float speed = 7.0;
float lastFrame = 0.0f;
//animation timer
float animTimer;
float angle;
void calculateTransformation() {
	//setting body matrix to identity
	bodyMat = glm::mat4(1.0);
	//position in x axis
	//bodyPos.x = sin(glfwGetTime())*10.;
	//position in y axis
	//bodyPos.z = cos(glfwGetTime())*10.;
	//bodyPos.x = 2.0;
	glm::vec3 position = camera.Position - bodyPos;

	angle = atan2(camera.Position.z - bodyPos.z, camera.Position.x - bodyPos.x);
	float speed = 0.01;

	bodyPos += position * glm::vec3(speed, speed, speed);

	//translating body matrix with it's position
	//bodyMat = glm::translate(bodyMat, bodyPos);
	double dArray[16] = { 0.0 };


	//rotating the body with timer angle in y axis (aka verticle axis)
	bodyMat = glm::rotate(bodyMat, -angle, glm::vec3(0.0, 1.0, 0.0));
	const float* pSource = (const float*)glm::value_ptr(bodyMat);
	for (int i = 0; i < 16; ++i) {
		dArray[i] = pSource[i];

		//cout << dArray[i] << "\n";
	}
	//translating head matrix with it's position and respect to body matrix...so it moves with body
	headMat = glm::translate(bodyMat, headPos - pos);
	pSource = (const float*)glm::value_ptr(headMat);
	for (int i = 0; i < 16; ++i) {
		dArray[i] = pSource[i];

		//cout << dArray[i] << "\n";
	}
	//translating left leg matrix
	LlegMat = glm::translate(bodyMat, LlegPos - pos);
	//rotating left leg matrix
	LlegMat = glm::rotate(LlegMat, (float)sin(animTimer), glm::vec3(0.0, 0.0, 1.0));

	pSource = (const float*)glm::value_ptr(LlegMat);
	for (int i = 0; i < 16; ++i) {
		dArray[i] = pSource[i];

		//cout << dArray[i] << "\n";
	}
	//translating right leg matrix
	//rotating right leg matrix
	RlegMat = glm::translate(bodyMat, RlegPos - pos);
	RlegMat = glm::rotate(RlegMat, (float)sin(animTimer - 3.1415), glm::vec3(0.0, 0.0, 1.0));

	pSource = (const float*)glm::value_ptr(RlegMat);
	for (int i = 0; i < 16; ++i) {
		dArray[i] = pSource[i];

		//cout << dArray[i] << "\n";
	}
	//translating left hand matrix
	//rotating right hand matrix
	LhandMat = glm::translate(bodyMat, LhandPos - pos);
	LhandMat = glm::rotate(LhandMat, (float)sin(animTimer - 3.1415), glm::vec3(0.0, 0.0, 1.0));
	pSource = (const float*)glm::value_ptr(LhandMat);
	for (int i = 0; i < 16; ++i) {
		dArray[i] = pSource[i];
		//cout << dArray[i] << "\n";
	}
	//translating right hand matrix
	//rotating  right hand matrix
	RhandMat = glm::translate(bodyMat, RhandPos - pos);
	RhandMat = glm::rotate(RhandMat, (float)sin(animTimer), glm::vec3(0.0, 0.0, 1.0));
	pSource = (const float*)glm::value_ptr(RhandMat);
	for (int i = 0; i < 16; ++i) {
		dArray[i] = pSource[i];
		//cout << dArray[i] << "\n";
	}
	animTimer += 0.1;
}

static void dropCallback(GLFWwindow* window, int path_count, const char* paths[]) {
	const char* path = paths[0];
	Model model(path);
	models.push_back(model);
	//float k = 1.;
}
int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, 1);
	glfwWindowHint(GLFW_DECORATED, true);
	//glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Animation", NULL, NULL);
	glfwSetWindowPos(window, 500, 100);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetDropCallback(window, dropCallback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------

	// build and compile shaders
	// -------------------------
	Shader ourShader("tree.vert", "tree.frag"); //tree shader
	Shader terrainShader("model.vert", "model.frag"); //ground shader
	Shader AnimationShader("animation.vert", "animation.frag"); //all body parts shader
	Shader CubeMapShader("cubemap.vert", "cubemap.frag");
	Shader ObjectShader("object.vert", "object.frag");
	// load models
	// -----------
	glEnable(GL_DEPTH_TEST);

	q.init();
	Model ourModel("model.obj");//tree 3d model
	Model terrain("terrain.obj");//ground 3d model
	Model head("head.obj");//head 3d model
	Model body("body.obj");//body 3d model
	Model Lleg("Lleg.obj");//left leg 3d model
	Model Rleg("Rleg.obj");//right leg 3d model
	Model Lhand("Lhand.obj");//left hand 3d model
	Model Rhand("Rhand.obj");//right hand 3d model
	Model cube("cube.obj");
	Model tank_body("tank_body.obj");
	Model tank_head("tank_head.obj");
	int tex = ourModel.LoadTexture("tree.png");//loading tree texture
	int tex1 = ourModel.LoadTexture("terrain.png");//loading ground texture
	int tex2 = ourModel.LoadTexture("dog.jpg");//loading ground texture

	vector<string> paths = { "skybox_posx.hdr","skybox_negx.hdr","skybox_posy.hdr","skybox_negy.hdr","skybox_posz.hdr","skybox_negz.hdr" };
	int cubeMap = cube.loadCubemap(paths);
	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// render loop
	// -----------

	ImGui::CreateContext();
	//ImGui::StyleColorsDark();
	const char* glsl_version = "#version 330";
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	float position[3] = { 0.0,0.0,0.0 };
	float Rotation[3] = { 0.0,0.0,0.0 };
	bool shift = 0;
	q.tex.id = tex2;
	float strength = 0.0f;
	float inputColor[4] = { 0.0,0.0,0.0,0.0 };
	float outputColor[3] = { 0.0,0.0,0.0 };
	float tank_rotation = 0.;
	string objects[]{ "terrain","tree","enderMan","tank" };
	int obje = 0;
	float colorScale[3] = { 0 };
	vector<const char*> n{ "terrain","tree","enderMan","Sky","tank" };
	while (!glfwWindowShouldClose(window))
	{

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		glm::mat4 vertMat(1.0);
		vertMat = glm::translate(vertMat, glm::vec3(0.0, 1.0, 0.0));
		calculateTransformation();//calculating all transformation of the scene
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		q.draw(glfwGetTime());

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);//projection matrix
		glm::mat4 view = camera.GetViewMatrix();//camera matrix aka view matrix
		glm::mat4 treeModelMatrix = glm::mat4(1.0f);
		glm::mat4 terrainModelMatrix = glm::mat4(1.0f);
		//////////////////////////////////////////

		ourShader.use();//tree shader use
		ourShader.setMat4("projection", projection);//tree projection
		ourShader.setMat4("view", view);//tree view matrix
		treeModelMatrix = glm::translate(treeModelMatrix, glm::vec3(0.0f, 0.0, 0.0f));//tree transformation matrix
		ourShader.setMat4("model", treeModelMatrix);//
		ourShader.setFloat("time", glfwGetTime());//sending timerr to tree for animation in shader
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(ourShader.ID, "tex"), 0);//sending texture to tree texture

		glBindTexture(GL_TEXTURE_2D, tex);
		ourModel.Draw(ourShader);//drawing tree
		////////////////////////////////////////


		CubeMapShader.use();
		CubeMapShader.setMat4("projection", projection);//terrain projection
		CubeMapShader.setMat4("view", view);//terrain camera matrix
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(CubeMapShader.ID, "skybox"), 0);
		glBindTexture(GL_TEXTURE_2D, cubeMap);
		cube.Draw(CubeMapShader);
		///////////////////////////////////////////////

		terrainShader.use();//terrain shader use
		terrainShader.setMat4("projection", projection);//terrain projection
		terrainShader.setMat4("view", view);//terrain camera matrix
		terrainShader.setMat4("model", terrainModelMatrix);//terrain model matrix or transformation matrix

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(terrainShader.ID, "tex"), 0);//sending texture to terrain shader

		glBindTexture(GL_TEXTURE_2D, tex1);

		terrain.Draw(terrainShader);//drawing terrain


		/////////////////////////////////////////////


		AnimationShader.use();//animation shader use
		AnimationShader.setMat4("projection", projection);//animated body projection
		AnimationShader.setMat4("view", view);//animated view matrix
		AnimationShader.setMat4("model", bodyMat);//animated body transformation matrix

		body.Draw(AnimationShader);//drawing body
		/////////////////////////////////////////////
		AnimationShader.use();

		AnimationShader.setMat4("model", headMat);//animated head transformation matrix

		head.Draw(AnimationShader);//drawing head
		/////////////////////////////////////////////
		AnimationShader.use();

		AnimationShader.setMat4("model", LlegMat);//animated left leg transformation matrix

		Lleg.Draw(AnimationShader);//drawing left leg
		/////////////////////////////////////////////
		AnimationShader.use();

		AnimationShader.setMat4("model", RlegMat);//animated right leg transformation matrix

		Rleg.Draw(AnimationShader);//drawing right leg
		/////////////////////////////////////////////
		AnimationShader.use();

		AnimationShader.setMat4("model", RhandMat);//animated right hand transformation matrix

		Rhand.Draw(AnimationShader);//drawing right hand
		/////////////////////////////////////////////
		AnimationShader.use();

		AnimationShader.setMat4("model", LhandMat);//animated left hand transformation matrix

		Lhand.Draw(AnimationShader);//drawing left hand
		/////////////////////////////////////////////

		glUseProgram(q.shaderProgram);
		glUniform1f(glGetUniformLocation(q.shaderProgram, "strength"), strength);
		glm::vec3 input = glm::vec3(inputColor[0], inputColor[1], inputColor[2]);
		glUniform3f(glGetUniformLocation(q.shaderProgram, "inp"), input.x, input.y, input.z);
		glUniform3f(glGetUniformLocation(q.shaderProgram, "outp"), outputColor[0], outputColor[1], outputColor[2]);
		glUniform3f(glGetUniformLocation(q.shaderProgram, "scale"), colorScale[0], colorScale[1], colorScale[2]);
		glUseProgram(0);

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			shift = 1;
		}
		else {
			shift = 0;
		}
		{
			float diff = angle - tank_rotation;
			tank_rotation += diff * 0.02;


			ObjectShader.use();
			ObjectShader.setMat4("projection", projection);//tree projection
			ObjectShader.setMat4("view", view);//tree view matrix
			int boolid = glGetUniformLocation(ObjectShader.ID, "boo");
			glUniform1i(boolid, 0);
			glm::mat4 modelMatrix(1.0);
			modelMatrix = glm::rotate(modelMatrix, -(3.1415f / 2.f), glm::vec3(1, 0, 0));
			ObjectShader.setMat4("model", modelMatrix);
			tank_body.Draw(ObjectShader);
			modelMatrix = glm::mat4(1.0);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.26326, 8.2965, 4.8335));
			modelMatrix = glm::rotate(modelMatrix, -tank_rotation - (3.1415f / 2.f), glm::vec3(0, 1, 0));
			modelMatrix = glm::rotate(modelMatrix, -(3.1415f / 2.f), glm::vec3(1, 0, 0));

			ObjectShader.setMat4("model", modelMatrix);
			tank_head.Draw(ObjectShader);

			for (Model model : models) {
				glm::mat4 modelMatrix(1.0);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(position[0], position[1], position[2]));
				modelMatrix = glm::rotate(modelMatrix, Rotation[0], glm::vec3(1, 0, 0));
				modelMatrix = glm::rotate(modelMatrix, Rotation[1], glm::vec3(0, 1, 0));
				modelMatrix = glm::rotate(modelMatrix, Rotation[2], glm::vec3(0, 0, 1));
				ObjectShader.setMat4("model", modelMatrix);
				model.Draw(ObjectShader);
			}

		}
		{
			static float f = 0.0f;
			f = position[0];
			static int counter = 0;

			ImGui::Begin("GUI");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);
			ImGui::TreeNode("jaja");
			const ImGuiTreeNodeFlags NodeFlags = 0;
			const bool NodeOpen = ImGui::TreeNodeEx("damn", NodeFlags, "Click me!");
			if (NodeOpen) ImGui::TreePop();
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit4("clear color", (float*)& clear_color); // Edit 3 floats representing a color

			ImGui::InputFloat("input", &f);

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		{

			ImGui::Begin("TRANSFORMATION");

			if (shift) {
				ImGui::DragFloat3("position", position, 1.0, 0.0, 0.0, "%3f");
			}
			else {
				ImGui::DragFloat3("position", position, 0.1, 0.0, 0.0, "%3f");
			}
			ImGui::DragFloat3("rotation", Rotation, 0.1, 0.0, 0.0, "%3f");

			ImGui::End();

		}


		{
			ImGui::Begin("Objects_List");
			static const char* it[5]{ "terrain","tree","enderMan","Sky","tank" };



			//	n.push_back("g");
				/*
				const size_t nn = str->size();
				const int p = 2;
				static  char cstr[] = "";
				str->copy(cstr, str->size() + 1);
				static const char* nes = str->c_str();
				const char** dd = n.data();
				*/
				//const char* kk = n[0].c_str();
				//const char** l = &kk;

				//static const char* d[] = {kk};
			int k = (glfwGetTime());
			int j = 15 % k;
			if (k % 15 == 1) {
				//string nam = string("haha") + std::to_string(k);
				const char* p = "jj";
				//n.push_back(p);
			}

			ImGui::ListBox("objects", &obje, &n[0], n.size());


			//ImGui::ListBox("obb", &obje, &itt,2);


			ImGui::End();


		}






		ImGui::Begin("ColorRelacer");

		ImGui::DragFloat("strength", &strength, 0.01, 0.0, 1.0);
		ImGui::DragFloat3("colorScale", colorScale, 0.01);
		ImGui::ColorPicker4("inputColor", &inputColor[0]);
		ImGui::ColorPicker3("outputColor", &outputColor[0]);
		//ImGui::ColorEdit3("name",inputColor);


		ImGui::End();



		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
	return 0;




}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime * speed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime * speed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime * speed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime * speed);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{




	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (ImGui::IsAnyItemActive()) {
			return;
		}
		//lastY = ypos;
		//firstMouse = true;
	}
	else {
		firstMouse = true;
		return;
	}

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);

}
