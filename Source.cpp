#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define GLFW_EXPOSE_NATIVE_WIN32
#include "stb_image_write.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_m.h"
#include "camera.h"
#include "stb_image.h"
using namespace std;
#include <windows.h>
#include "quad.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include "Lights.h"
#include "LineRenderer.h"
#include <GLFW\glfw3native.h>
#include <cmath>
using namespace glm;

#include "Scene.h"
#include "Events.h"
#include "Action.h"
#include "Gui.h"



bool esc = false;
float mxposition;
float myposition;
float mxoffset;
float myoffset;
int action_axis;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void mouseCallback(GLFWwindow*, int, int, int);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
static void keyCallback(GLFWwindow*, int, int, int, int);
void pickObject(void);
void window_focus_callback(GLFWwindow* window, int focused);
GLFWwindow *window;

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;
int width = SCR_WIDTH, height = SCR_HEIGHT;
// camera
static Camera camera(glm::vec3(0.0f, 1.4f, 4.1859f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float lastwx = 0;
float lastwy = 0;
float lastwz = 0;
bool firstMouse = true;
bool WindowActive = true;
bool WindowFocus = true;

Scene scene;
Action act;
float deltaTime = 0.0f;
float speed = 7.0;
float lastFrame = 0.0f;
static int previous = -1;
Gui gui;
bool shift = false;
bool ctrl = false;
bool alt = false;
static void dropCallback(GLFWwindow* window, int path_count, const char* paths[]) {
	scene.LoadObject(path_count, paths);
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
	//glfwWindowHint(GLFW_DECORATED, false);
	glfwWindowHint(GLFW_RESIZABLE, 1);
	//glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
	
	

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Boolean Game Engine", NULL, NULL);
	glfwSetWindowPos(window, 500, 100);
	GLFWimage icons[1];
	int width, height, nrComponents;
 
	icons[0].pixels = stbi_load("Data/icon_boolean_2.png", &icons[0].width, &icons[0].height, 0, 4);
	glfwSetWindowIcon(window, 1, icons);
	stbi_image_free(icons[0].pixels);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, keyCallback);

	glfwSetDropCallback(window, dropCallback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);//projection matrix
	camera.setProjectionMatrix(SCR_WIDTH, SCR_HEIGHT);
	// configure global opengl state

	//pickingShader = Shader("Data/picking.vert", "Data/picking.frag");
	
	TCHAR szExeFileName[MAX_PATH];
	GetModuleFileName(NULL, szExeFileName, MAX_PATH);
	std::cout << szExeFileName << "\n";
	// load models
	// -----------
	//camera.width = SCR_WIDTH;
	//camera.height = SCR_HEIGHT;
	width = SCR_WIDTH;
	height = SCR_HEIGHT;
	scene = Scene(camera);
	act.init(scene);
	scene.width = SCR_WIDTH;
	scene.height = SCR_HEIGHT;
	scene.window = window;

	glEnable(GL_DEPTH_TEST);

	gui.init(window);
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	float kx = 0.;
	float ky = 0.;
	//map<string, uint> bone;
	//bone["hehe"] = 1;
	while (!glfwWindowShouldClose(window))
	{
		
			double x1, y1;
			glfwGetCursorPos(window, &x1, &y1);

			glfwSetWindowFocusCallback(window, window_focus_callback);
			if (act.action != Action::Nothing) {
				std::cout << width << " width" << "\n";
				if (x1 >= scene.width) {
					glfwSetCursorPos(window, 0, y1);
					firstMouse = true;
				}
				else if (x1 <= 0) {
					glfwSetCursorPos(window, scene.width, y1);
					firstMouse = true;
				}

				if (y1 >= scene.height) {
					glfwSetCursorPos(window, x1, 0);
					firstMouse = true;
				}
				else if (y1 <= 0) {
					glfwSetCursorPos(window, x1, scene.height);
					firstMouse = true;
				}
			}
			gui.start();
			glm::mat4 vertMat(1.0);
			vertMat = glm::translate(vertMat, glm::vec3(0.0, 1.0, 0.0));

			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			kx += mxoffset;
			ky += myoffset;
			float xx = sin(glfwGetTime());
			float xy = cos(glfwGetTime());

			processInput(window);
			float x = glm::sin(glfwGetTime());
			float y = 0.;
			//camera.Position.x = sin(glfwGetTime()) *camera.Zoom;
			//camera.Position.z = cos(glfwGetTime()) * camera.Zoom;

			if (WindowActive && WindowFocus) {

				scene.draw(camera);
			}
			act.draw(camera);
			act.runAction(scene);

			
			///////////////////////////////////////GUI///////////////////////////////////
			if (glfwGetTime() > 5. || 1)
			{
				gui.draw(scene);
				gui.render();
				glfwSwapBuffers(window);
				glfwPollEvents();
			}

	}
	
	gui.terminate();
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	//if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		//esc = true;
	float k = 0;

	//	camera.ProcessKeyboard(RIGHT, deltaTime * speed);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		shift = true;
	else
		shift = false;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		ctrl = true;
	else
		ctrl = false;
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
		alt = true;
	else
		alt = false;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
//	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)w / (float)h, 0.1f, 1000.0f);//projection matrix
	if(!w<=0||!h<=0){
		WindowActive = true;
	camera.setProjectionMatrix( w,h);
	scene.fbo.update(w, h,scene.fbo.num);
	scene.fbo2.update(w, h,scene.fbo2.num);
	scene.bloom1.update(w/16, h/16, scene.bloom1.num);
	scene.bloom2.update(w/8, h/8, scene.bloom2.num);
	scene.bloom3.update(w/2, h/2, scene.bloom3.num);
	scene.bloom4.update(w/2, h/2, scene.bloom4.num);
	//glViewport(0, 0, w, h);
	width = w;
	height = h;
	scene.width = w;
	scene.height = h;
	}
	else {
		WindowActive = false;
	}
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//std::cout << xpos << "\n";
		//std::cout << width << "\n";
	glm::vec3 worldPos = camera.unProject(xpos, ypos);
	//std::cout << worldPos.x << " " << worldPos.y << " " << worldPos.z << "\n";
	
	float n = 1.0;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		lastwx = worldPos.x;
		lastwy = worldPos.y;
		lastwz = worldPos.z;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	float wxoffset = worldPos.x - lastwx;
	float wyoffset = worldPos.y - lastwy;
	float wzoffset = worldPos.z - lastwz;
	if (scene.objects.size() > 0) {

	//	scene.objects[scene.selectionIndex].position += glm::vec3(wxoffset, wyoffset, wzoffset)*glm::length(camera.Position- scene.objects[scene.selectionIndex].position);
		
		}
	lastwx = worldPos.x;
	lastwy = worldPos.y;
	lastwz = worldPos.z;
	lastX = xpos;
	lastY = ypos;
	mxoffset = xoffset*0.7;
	myoffset = yoffset*0.7;
	mxposition = xpos/width;
	myposition = 1.-(ypos/height);



	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		if (ImGui::IsAnyItemActive()) {

			return;
		}
		//cout << xpos << "\n";
		if (xpos > width ) {
			glfwSetCursorPos(window, 0, ypos);
			firstMouse = true;
		}
		else if (xpos <= 0) {
			glfwSetCursorPos(window, width, ypos);
			firstMouse = true;
		}

		if (ypos > height) {
			glfwSetCursorPos(window, xpos, 0);
			firstMouse = true;
		}
		else if (ypos <= 0) {
			glfwSetCursorPos(window, xpos, height);
			firstMouse = true;
		}


		
		if (shift) {
			
			camera.dragCamera(xoffset*0.01, yoffset*0.01);
		}
		else {

			camera.ProcessMouseMovement(xoffset , yoffset );
		}
	}
	else {
		//firstMouse = true;
		return;
	}
	
	
}
static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		
		act.ActionClose();
		if (!ImGui::IsAnyItemHovered()) {

			pickObject();
		}
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);

}
void pickObject(void)
{
	
	scene.pickObject();


}
void window_focus_callback(GLFWwindow* window, int focused)
{
	if (focused)
	{
		WindowFocus = true;// The window gained input focus
	}
	else
	{
		WindowFocus = false;
		
	}

}
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
	if (!ImGui::IsAnyItemActive()) {
		std::cout <<shift << "\n";




		if (action == GLFW_PRESS) {
			camera.setKeyPositions(key);

			switch (key)
			{
			case GLFW_KEY_ESCAPE:
				esc = true;
				//act.ActionClose();
				break;
			case GLFW_KEY_A:
				break;
			case GLFW_KEY_D:
				if (shift)
				{
					scene.DuplicateObject();
				}
				break;

			case GLFW_KEY_G:

				act.action = Action::Move;
				break;
			case GLFW_KEY_ENTER:
				act.ActionClose();
				break;
			case GLFW_KEY_X:
				act.axis = Action::X;
				break;
			case GLFW_KEY_Y:
				act.axis = Action::Z;
				break;
			case GLFW_KEY_Z:
				act.axis = Action::Y;
				break;
			case GLFW_KEY_R:
				act.action = Action::Rotate;
				break;
			case GLFW_KEY_S:
				act.action = Action::Scale;
				break;
			case GLFW_KEY_0:
				act.addNumbers("0");
				break;
			case GLFW_KEY_1:
				act.addNumbers("1");
				break;
			case GLFW_KEY_2:
				act.addNumbers("2");
				break;
			case GLFW_KEY_3:
				act.addNumbers("3");
				break;
			case GLFW_KEY_4:
				act.addNumbers("4");
				break;
			case GLFW_KEY_5:
				act.addNumbers("5");
				break;
			case GLFW_KEY_6:
				act.addNumbers("6");
				break;
			case GLFW_KEY_7:
				act.addNumbers("7");
				break;
			case GLFW_KEY_8:
				act.addNumbers("8");
				break;
			case GLFW_KEY_9:
				act.addNumbers("9");
				break;
			case GLFW_KEY_KP_DECIMAL:
				if (act.action == Action::Nothing) {
					glm::vec3 scal = scene.objects[scene.selectionIndex].scale;
					
					glm::vec3 dim = scene.allPrimitives[scene.objects[scene.selectionIndex].primitiveID].mesh.Dimensions;
					glm::vec3 fDim = dim * scal;
					cout << fDim.x << "\n"; cout << fDim.y << "\n"; cout << fDim.z << "\n";
					if (scene.objects.size())
						camera.setOrigin(scene.objects[scene.selectionIndex].position, fDim);
				}

				act.addNumbers(".");
				break;
			case GLFW_KEY_PERIOD:
				act.addNumbers(".");
				break;
			case GLFW_KEY_KP_5:
				camera.setOrtho();
				camera.setProjectionMatrix(width, height);
				break;

			case GLFW_KEY_KP_7:
				//camera.setOrtho();

				//camera.setProjectionMatrix(width, height);
				break;
			case GLFW_KEY_SPACE:
				if (scene.sceneLights.panelStatus) {
					scene.sceneLights.panelStatus = false;
				}
				else {
					scene.sceneLights.panelStatus = true;
				}

			}

		}
	}
}
