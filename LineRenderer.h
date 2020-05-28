#pragma once

class LineRenderer {
public:
	unsigned int lineVAO, lineVBO;
	Shader lineShader;

	LineRenderer(glm::vec3 start,glm::vec3 end) {
		 lineShader = Shader("Data/line.vert", "Data/line.frag");
		 std::cout << lineShader.ID << "\n";
		
		float lineVertices[] = {
			start.x,start.y,start.z,end.x,end.y,end.z	
		};
		
		glGenVertexArrays(1, &lineVAO);
		glGenBuffers(1, &lineVBO);
		glBindVertexArray(lineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), &lineVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	

		// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
		glBindVertexArray(0);
	}

	LineRenderer(float *array) {

	}

	LineRenderer() {

	}
	void draw(Camera cam, glm::vec3 offset,int lineCount,glm::vec3 color = glm::vec3(0.8,0.8,0.8),glm::vec3 position = glm::vec3(0.0), glm::vec4 rotate = glm::vec4(1.0,1.,1.,0.)) {
		glUseProgram(lineShader.ID);
		glBindVertexArray(lineVAO); 
		lineShader.setMat4("projection", cam.projMat);
		lineShader.setMat4("view", cam.GetViewMatrix());
		lineShader.setVec3("color", color);
	
		for (int i = 0; i < lineCount; i++) {
			
			glm::mat4 modelMat(1.0);
			//float si = i - (lineCount / 2.);
			modelMat = glm::translate(modelMat, position);
			modelMat = glm::rotate(modelMat,rotate.w,glm::vec3(rotate));
			modelMat = glm::translate(modelMat, glm::vec3(offset)*glm::vec3(i));
			lineShader.setMat4("model",modelMat);
			glDrawArrays(GL_LINES, 0, 2);
		}
		
		glUseProgram(0);
	
	}

};
