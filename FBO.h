#pragma once
class FBO {
public:
	unsigned int ColorTexture;
	std::vector<unsigned  int> textures;
	unsigned int framebuffer;
	unsigned int width;
	unsigned int height;
	unsigned int num;
	FBO(int w, int h, const int ColorNum = 1,bool depthOnly = false) {
		num = ColorNum;
		if (depthOnly) {
			initializeDepthOnly(w, h);
		}
		else {
			initialize(w, h, ColorNum);
		}
		

	}
	FBO() {

	}

	void initialize(int w, int h, const int ColorNum) {
		this->width = w;
		this->height = h;
		glGenFramebuffers(1, &framebuffer);
		//GL_CHECK(glGenFramebuffers(1, &framebuffer));
		for (int i = 0; i < ColorNum; i++) {
			unsigned int ColorTexture;
			glGenTextures(1, &ColorTexture);
			glBindTexture(GL_TEXTURE_2D, ColorTexture);
	
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,width,height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			textures.push_back(ColorTexture);
		}

		ColorTexture = textures.at(0);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		int k = 0;

		std::vector<unsigned int> attacted;
		for (unsigned int texture : textures) {
			// glBindTexture(GL_TEXTURE_2D,texture);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + k, GL_TEXTURE_2D, texture, 0);

			attacted.push_back(GL_COLOR_ATTACHMENT0 + k);

			k++;
		}

		glDrawBuffers(textures.size(), &attacted[0]);
		//glEnablei(GL_BLEND, framebuffer);
		//glBlendFunci(framebuffer, GL_ONE, GL_ZERO);
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
		bool flag = glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void initializeDepthOnly(int w, int h ) {
		this->width = w;
		this->height = h;
		glGenFramebuffers(1, &framebuffer);
		//GL_CHECK(glGenFramebuffers(1, &framebuffer));
		
		unsigned int depthMap;
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			textures.push_back(depthMap);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);


	}
	void update(int w, int h, const int ColorNum = 1) {
		for (auto& tex : textures) {
			//glDeleteTextures(1, &tex);
		}
		//glDeleteFramebuffers(1, &framebuffer);
	//	initialize(w, h, ColorNum);



	}


	void bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, width, height);
	}
	void ubind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	unsigned int getColorTexture() {

		return textures.at(0);
	}



};
