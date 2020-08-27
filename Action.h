
class Action {
public:
	enum ActionStates { Nothing, Move, Rotate, Scale, Delete } action = Nothing;
	enum Axis { NUL, X, Y, Z } axis = NUL;
	bool firstAction = true;
	glm::vec3 currenRotation = glm::vec3(0., 0., 0.);
	//LineRenderer line3;
	Scene* scene;
	glm::vec3 currentPosition = glm::vec3(0.);
	glm::vec3 firstPosition = glm::vec3(0.);
	glm::vec3 firstRotation = glm::vec3(0.);
	glm::vec3 firstScale = glm::vec3(0.);
	float oldAngle = 0.0;
	float oldDistance = 0.0;
	string numbers;
	void init(Scene &scene) {
		scene = scene;
		//line3 = LineRenderer(vec3(0, -1000, 0), vec3(0, 1000, 0));

	}
	
	void runAction(Scene &scene) {
		if (action != Nothing) {
			float xpos = mxoffset*0.05;
			float ypos = myoffset*0.05;

			if (scene.objects.size() > 0) {
				
				glm::vec2 obPos = scene.objects[scene.selectionIndex].screenPos;
				vec2 mpos = (glm::vec2(mxposition, myposition));
				float an = atan2(obPos.y - mpos.y, obPos.x - mpos.x);
				if (firstAction) {
					firstRotation = scene.objects[scene.selectionIndex].rotation;
					firstPosition = scene.objects[scene.selectionIndex].position;
					firstScale = scene.objects[scene.selectionIndex].scale;
					float distan = distance(mpos, obPos);
					//float offset = distan - oldDistance;
					oldDistance = distan;
					//currenRotation = scene.objects[scene.selectionIndex].rotation;
					firstAction = false;
				}
				currentPosition = scene.objects[scene.selectionIndex].position;
				if (action == Move) {
					if (axis == NUL) {
						scene.objects[scene.selectionIndex].position.x -= (scene.camera->cameraVector.z) * xpos * scene.camera->Zoom * 0.03;
						scene.objects[scene.selectionIndex].position.x += (scene.camera->cameraVector.x) * ypos * scene.camera->Zoom * 0.03;

						scene.objects[scene.selectionIndex].position.z += (scene.camera->cameraVector.x) * xpos * scene.camera->Zoom * 0.03;
						scene.objects[scene.selectionIndex].position.z += (scene.camera->cameraVector.z) * ypos * scene.camera->Zoom * 0.03;

						scene.objects[scene.selectionIndex].position.y += ypos * scene.camera->Zoom * 0.03;
						//scene.objects[scene.selectionIndex].position +=
						//scene.objects[scene.selectionIndex].position += scene.camera->Right * glm::vec3(xpos, xpos, xpos);
						//scene.objects[scene.selectionIndex].position += scene.camera->Front * glm::vec3(ypos, ypos, ypos);
						//scene.objects[scene.selectionIndex].position += scene.camera->Up * glm::vec3(ypos, ypos, ypos);
						//objects[selectionIndex].position += camera->Up * glm::vec3(ypos,ypos,ypos);
					}
					else if (axis == X) {
						//scene.camera->cameraVector
						
						scene.objects[scene.selectionIndex].position.x -= (scene.camera->cameraVector.z) * xpos* scene.camera->Zoom*0.03;
						scene.objects[scene.selectionIndex].position.x += (scene.camera->cameraVector.x) * ypos * scene.camera->Zoom*0.03;
						//scene.objects[scene.selectionIndex].position.x += scene.camera->Front.x * ypos;
						if (numbers.size() > 0) {
							scene.objects[scene.selectionIndex].position.x = firstPosition.x+std::stof(numbers);
						}
					}
					else if (axis == Z) {
						scene.objects[scene.selectionIndex].position.z += (scene.camera->cameraVector.x) * xpos * scene.camera->Zoom * 0.03;
						scene.objects[scene.selectionIndex].position.z += (scene.camera->cameraVector.z) * ypos * scene.camera->Zoom * 0.03;
						if (numbers.size() > 0) {
							scene.objects[scene.selectionIndex].position.z = firstPosition.z+ std::stof(numbers);
						}
					}
					else if (axis == Y) {
						scene.objects[scene.selectionIndex].position.y +=ypos * scene.camera->Zoom * 0.03;
						//scene.objects[scene.selectionIndex].position.y += scene.camera->Up.y * ypos;
						//scene.objects[scene.selectionIndex].position.y += scene.camera->Front.y * xpos;
						//scene.objects[scene.selectionIndex].position.y += scene.camera->Right.y * xpos;
						if (numbers.size() > 0) {
							scene.objects[scene.selectionIndex].position.y = firstPosition.y+std::stof(numbers);
						}
						//glm::rotate(2.3, glm::vec3(1., 0., 0.));
					}
				}
				else if (action == Rotate) {
					if (firstAction) {
						firstAction = false;
						oldAngle = an;
					}
					float offset = an - oldAngle;
					oldAngle = an;
					glm::vec3 direction = normalize(scene.objects[scene.selectionIndex].position - scene.camera->Position);
					if (axis == NUL) {

						float speed = 2.0;
						//scene.objects[scene.selectionIndex].rotation.z -= offset * (180 / 3.1415) * scene.camera->Front.z;
						//scene.objects[scene.selectionIndex].rotation.x -= offset * (180 / 3.1415) * scene.camera->Front.x;
						//scene.objects[scene.selectionIndex].rotation.y -= offset * (180 / 3.1415) * scene.camera->Front.y;

					}
					else if (axis == X) {
						scene.objects[scene.selectionIndex].rotation.x -= offset * (180 / 3.1415)*glm::sign(direction.x+0.001);
						if (numbers.size() > 0) {
							scene.objects[scene.selectionIndex].rotation.x = std::stof(numbers);
						}
						
					
					}
					else if (axis == Z) {
						scene.objects[scene.selectionIndex].rotation.z -= offset * (180 / 3.1415) * glm::sign(direction.z + 0.001);
						if (numbers.size() > 0) {
							scene.objects[scene.selectionIndex].rotation.z = std::stof(numbers);
						}
						
					}
					else if (axis == Y) {
						scene.objects[scene.selectionIndex].rotation.y -= offset * (180 / 3.1415) * glm::sign(direction.y + 0.001);
						if (numbers.size() > 0) {
							scene.objects[scene.selectionIndex].rotation.y = std::stof(numbers);
						}
						
					}

					
				}
				else if (action == Scale) {
					if (firstAction) {
						//firstAction = false;
						//oldAngle = an;
					}
					if (axis == NUL) {
						glm::vec2 obPos = scene.objects[scene.selectionIndex].screenPos;
						vec2 mpos = (glm::vec2(mxposition, myposition));
						float distan = distance(mpos, obPos);
						float offset = distan - oldDistance;
						oldDistance = distan;
						scene.objects[scene.selectionIndex].scale *= 1+offset;
					
					}

				}
				if (esc) {
					scene.objects[scene.selectionIndex].rotation = firstRotation;
					scene.objects[scene.selectionIndex].position = firstPosition;
					scene.objects[scene.selectionIndex].scale = firstScale;
					ActionClose();
					esc = false;
				}
				mxoffset = 0.;
				myoffset = 0.;
			}
		}

	}
	void draw(Camera &camera) {
		glLineWidth(1.0);
		if (axis == Action::Y) {
			action_axis = 2;
			//line3.draw(camera, vec3(0, 0, 0), 1, vec3(0., 0., 1.), currentPosition);
		}
		else if (axis == Action::X) {
			action_axis = 1;
			/*
			glm::mat4 model(1.0);
			glm::translate(model,currentPosition);
			glm::rotate(model, 1.5708f, glm::vec3(0, 0, 1));
			line3.draw(camera, vec3(0, 0, 0), 1, vec3(1., 0., 0.), currentPosition, glm::vec4(0, 0, 1, 1.5708f));
			*/
		}
		else if (axis == Action::Z) {
			action_axis = 3;
			/*
			
			glm::mat4 model(1.0);
			line3.draw(camera, vec3(0, 0, 0), 1, vec3(0., 1., 0.), currentPosition, glm::vec4(1, 0, 0, 1.5708f));
			*/
		}
		else if (axis == Action::NUL) {
			action_axis = 0;
		}
		glLineWidth(1.0);
	}
	void ActionClose() {
		if (action != Nothing) {
			firstAction = true;
		}
		
		numbers.clear();
		action = Nothing;
		axis = NUL;
		
		
	}
	void addNumbers(string number) {
		if (action == Rotate || action == Move) {
			numbers += number;
		}
		
	}

};