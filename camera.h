#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Events.h"
#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	glm::vec3 objectTrackPosition;
	glm::vec3 cameraVector;
	bool objectTacking;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	float width;
	float height;
	float x = 0.;
	float zoomSpeed = 3.;
		float y = 0.5;
	//camera Perspective projection
	glm::mat4 projMat;
	bool ortho = false;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		//Position = position;
		Up = glm::vec3(0., 1., 0.);
		calculatePosition();
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
		cameraVector = normalize(objectTrackPosition - Position);
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		//calculatePosition();
		
		Yaw += 2.;
		Pitch += 2.;
		
		
		if (1) {
			//cameraVector = normalize(objectTrackPosition - Position);
		
			return glm::lookAt(Position, objectTrackPosition,Up);
		}
		
		else {
			return glm::lookAt(Position, Position + Front, glm::vec3(0.,1.,0.));
		}
		
		
	}
	void setProjectionMatrix(int w ,int h) {
		width = w;
		height = h;
		projMat = glm::perspective(45.f, (float)w / (float)h, 0.1f, 1000.0f);//projection matrix
		
		if (ortho) {
			float ratio = Zoom;
			
			float aspect = (width / height);
			float x = aspect *Zoom;
			float y = 1*Zoom;
			projMat = glm::ortho(-x, x, -y, y, 0.1f, 1000.0f);
			
		}
		calculatePosition();
		//projMat = proj;
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		std::cout << x << " " << y << " name\n";
		cameraVector = normalize(objectTrackPosition - Position);
		x -= xoffset*0.01;
		y -= yoffset*0.01;
		calculatePosition();
		Position += objectTrackPosition;
		
		//Up = glm::vec3(0., 1., 0.);
	//	Position.x = sin(xoffset*0.01);
	//	Position.z = cos(xoffset*0.01);
		
		
		
		objectTacking = false;
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;
		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		
		
		
		if (Zoom >= 1.0f && Zoom <= 2245.0f)
			if(alt)
				Zoom -= yoffset*zoomSpeed*0.1;
			else
				Zoom -= yoffset * zoomSpeed;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 2245.0f)
			Zoom = 2245.0f;
		calculatePosition();
		Position += objectTrackPosition;
		if (ortho) {
			float ratio = Zoom;

			float aspect = (width / height);
			float x = aspect * Zoom;
			float y = 1 * Zoom;
			projMat = glm::ortho(-x, x, -y, y, -0.f,100.f);
			
		}
			
	}

	void setOrigin(glm::vec3 pos,glm::vec3 dim) {
		float max = 0.;
		if (dim.x > max) {
			max = dim.x;
			if (dim.y > max) {
				max = dim.y;
			}
			else if (dim.z > max) {
				max = dim.z;
			}
		}
	//	Zoom = max + 4.;
		objectTrackPosition = pos;
		calculatePosition();
		Position += objectTrackPosition;
		
	}
	

	void dragCamera(float xoffset, float yoffset) {
		std::cout << x << " " << y << " name\n";
		objectTrackPosition.x += (cameraVector.z) * xoffset * Zoom * 0.03;
		objectTrackPosition.x -= (cameraVector.x) * yoffset * Zoom * 0.03;

		objectTrackPosition.z -= (cameraVector.x) * xoffset * Zoom * 0.03;
		objectTrackPosition.z -= (cameraVector.z) * yoffset * Zoom * 0.03;

		objectTrackPosition.y -= yoffset * Zoom * 0.03;
		calculatePosition();
		
		Position += objectTrackPosition;
		

	}
	

	void dragCamera(float xoffset, float yoffset,float zoffset) {
		/*
		objectTrackPosition.x += (cameraVector.z) * xoffset * Zoom * 0.03;
		objectTrackPosition.x -= (cameraVector.x) * yoffset * Zoom * 0.03;

		objectTrackPosition.z -= (cameraVector.x) * xoffset * Zoom * 0.03;
		objectTrackPosition.z -= (cameraVector.z) * yoffset * Zoom * 0.03;

		objectTrackPosition.y -= yoffset * Zoom * 0.03;
		calculatePosition();

		Position += objectTrackPosition;
		*/
		//scene.objects[scene.selectionIndex].position += glm::vec3(wxoffset, wyoffset, wzoffset) * glm::length(camera.Position - scene.objects[scene.selectionIndex].position);
		objectTrackPosition-= glm::vec3(xoffset, yoffset, zoffset)* glm::length(Position - objectTrackPosition);
		//calculatePosition();
		Position += objectTrackPosition;
	
	}
	void setOrtho() {
		if (ortho) {
			ortho = false;
		}
		else {
			ortho = true;
		}
		
	}
	void setKeyPositions(int key) {
		
		
		if (key == GLFW_KEY_KP_7) {
			x = 0.;
			y = 1.5708;
			if (ctrl) {
				y = -1.5708;
			}
			calculatePosition();
			setProjectionMatrix(width, height);
			//calculatePosition();
		
			
		}
		if (key == GLFW_KEY_KP_3) {
			x = 1.5708;
			y = 0.;
			if (ctrl) {
				x = 4.71239;
			}
			calculatePosition();
			setProjectionMatrix(width, height);
			//calculatePosition();
			
		}

			if (key == GLFW_KEY_KP_1) {
				x = 0.;
				y = 0.;
				if (ctrl) {
					x = 3.1415;
				}
				calculatePosition();
				setProjectionMatrix(width, height);
			}
		
		
			
	}
	glm::vec3 unProject(float mx,float my) {
		// these positions must be in range [-1, 1] (!!!), not [0, width] and [0, height]
		float mouseX =mx / (width * 0.5f) - 1.0f;
		float mouseY = my / (height * 0.5f) - 1.0f;
		glm::mat4 invVP = glm::inverse(projMat * GetViewMatrix());
		glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
		glm::vec4 worldPos = invVP * screenPos;

		glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

		return glm::vec3(worldPos);
	}


private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{

		// Calculate the new Front vector
	

		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		//Up = glm::normalize(glm::cross(Right, Front));
		
	}
	void calculatePosition() {
		Position.x = sin(x) * Zoom * cos(y);
		Position.z = cos(x) * Zoom * cos(y);
		Position.y = sin(y) * Zoom;
		if (abs(y) > 3.1415 / 2 && abs(y) < 4.71239) {
			Up = glm::vec3(0., -1., 0.);
		}
		else {
			Up = glm::vec3(0., 1., 0.);
		}
	}
};
#endif