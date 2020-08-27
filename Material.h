
class Material {
public:
	enum Mode{Default,Textured, PBR,Lighting,PBR_T};
	Mode mode = Default;
	bool bloom = false;
	bool bloomTexture = false;
	unsigned int DiffuseTextureID = 0;
	unsigned int NormalTextureID = 0;
	unsigned int GlossyTextureID = 0;
	unsigned int RoughnessTextureID = 0;
	unsigned int AmbientTextureID = 0;
	unsigned int bloomTextureID = 0;
	unsigned int brdfTexture = 0;
	unsigned int environmentTexture = 0;
	unsigned int irradianceTexture = 0;
	float roughness = 0.0;
	float metalness = 0.0;
	float ambient = 0.0;
	Camera *cam;
	bool blending = false;
	Shader *shad;
	glm::vec4 objectColor = vec4(1.);
	vector<unsigned int> *ObjectTextures;
	vector<Texture>* sceneTextures;
	Lights *scnLight;
	Material(Mode &matmode) {
		mode = matmode;

	}
	Material() {

	}
	void Update(Mode matmode = Default) {
		mode = matmode;
	}

	void set(std::vector<Shader> &shaders,Lights &sceneLight,vector<Texture> &scnTextures,vector<unsigned int> &objTextures,Camera *camera) {
		ObjectTextures =&objTextures;
		sceneTextures = &scnTextures;
		scnLight = &sceneLight;
		cam = camera;
		if (mode == Default) {
			shad = &shaders[0];
		}
		else if (mode == Textured) {
			shad = &shaders[1];
		}
		else if (mode == PBR) {
			shad = &shaders[2];
		}
		else if (mode == Lighting) {
			shad = &shaders[3];
		}
		else if (mode == PBR_T) {
			shad = &shaders[4];
		}
	}
	void start() {
		shad->use();
		glm::vec3 pos[3];
		pos[0] = glm::vec3(3., 0., 0.);
		pos[1] = glm::vec3(0., 5., 0.);
		pos[2] = glm::vec3(-3., 0., 0.);

		for (unsigned int i = 0; i < scnLight->lights.size(); i++) {
			shad->setVec3("lightPositions[" + std::to_string(i) + "]", scnLight->lights[i].position);
			shad->setVec3("lightColors[" + std::to_string(i) + "]", scnLight->lights[i].color*scnLight->lights[i].strength);
		}
		shad->setInt("LightSize", scnLight->lights.size());
		shad->setBool("blending", blending);


		for (unsigned int j = 0; j < 3; j++) {
			//shad->setVec3("lightPositions[" + std::to_string(j) + "]", pos[j]);
			//shad->setVec3("lightColors[" + std::to_string(j) + "]",glm::vec3(300.));
		}
	//	shad->setInt("LightSize", 3);



		if (mode == Default) {
			shad->setVec4("objectColor", objectColor);
			if (bloom) {
				shad->setBool("bloom", true);
				if (bloomTexture) {
					if (ObjectTextures->size() > 0) {
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, sceneTextures->at(ObjectTextures->at(bloomTextureID)).id);
						shad->setBool("GlowTexture", 0);
						shad->setBool("BloomTexture", 1);
					}
					else {
						shad->setBool("BloomTexture", 0);
					}
				}
			}
			else {
				shad->setBool("bloom", false);
				shad->setBool("BloomTexture", 0);
			}

		}
		else if (mode == Textured) {
			if (ObjectTextures->size() > 0) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, sceneTextures->at(ObjectTextures->at(DiffuseTextureID)).id);
				shad->setInt("DiffuseTexture", 0);
				if (bloom) {
					shad->setBool("bloom", true);
					if (bloomTexture) {
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, sceneTextures->at(ObjectTextures->at(bloomTextureID)).id);
						shad->setInt("GlowTexture", 2);
						shad->setBool("BloomTexture", 1);
					}
					else {
						shad->setBool("BloomTexture", 0);
					}
				}
				else {
					shad->setBool("bloom", false);
					shad->setBool("BloomTexture", 0);
				}
			}
		}
		else if (mode == PBR) {
			if (ObjectTextures->size() > 0) {
				//glActiveTexture(GL_TEXTURE0);
				//glBindTexture(GL_TEXTURE_2D, sceneTextures->at(ObjectTextures->at(DiffuseTextureID)).id);
				//shad->setInt("DiffuseTexture", 0);
			
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, sceneTextures->at(ObjectTextures->at(environmentTexture)).id);
				shad->setInt("envMap", 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, sceneTextures->at(ObjectTextures->at(DiffuseTextureID)).id);
				shad->setInt("DiffuseTexture", 1);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, sceneTextures->at(ObjectTextures->at(GlossyTextureID)).id);
				shad->setInt("glossyTexture", 2);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, sceneTextures->at(ObjectTextures->at(RoughnessTextureID)).id);
				shad->setInt("roughnessTexture", 3);
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, sceneTextures->at(ObjectTextures->at(NormalTextureID)).id);
				shad->setInt("normalMap", 4);
				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, sceneTextures->at(ObjectTextures->at(bloomTextureID)).id);
				shad->setInt("GlowTexture", 5);
				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_CUBE_MAP, sceneTextures->at(ObjectTextures->at(irradianceTexture)).id);
				shad->setInt("IrradianceTexture", 6);
				glActiveTexture(GL_TEXTURE7);
				glBindTexture(GL_TEXTURE_2D, sceneTextures->at(ObjectTextures->at(brdfTexture)).id);
				shad->setInt("brdfTexture", 7);
				glActiveTexture(GL_TEXTURE8);
				glBindTexture(GL_TEXTURE_2D, sceneTextures->at(ObjectTextures->at(AmbientTextureID)).id);
				shad->setInt("AoTexture", 8);
				

				shad->setVec3("camPos", cam->Position);
				shad->setFloat("roughness", roughness);
				shad->setFloat("metalness", metalness);
				shad->setFloat("Ambient", ambient);
				
				if (bloom) {
					shad->setBool("bloom", true);
					if (bloomTexture) {
						shad->setBool("BloomTexture", 1);
					}
					else {

						shad->setBool("BloomTexture", 0);
					}
				}
				else {
					shad->setBool("bloom", false);
					//shad->setBool("BloomTexture", 0);
				}
			}


		}
	}


};
