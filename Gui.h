#include <Shlwapi.h>
#include <tchar.h>
#include <shlobj.h> 
#include "imgui_stdlib.h"
class Gui {
GLFWwindow *window;
public:
	int count = 0;
	//int 
	void init(GLFWwindow* window) {
		this->window = window;
		ImGui::CreateContext();
		const char* glsl_version = "#version 330";
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
	}
	void start() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	void openLightPanel(Scene &scene) {

		ImGui::Begin("LightPanel");
		string types[3] = { "point","directionalLight","spotLight"};
		int k = 0;
		for (int i = 0; i < 3;i++) {
			ImGui::Selectable(types[i].c_str(), scene.sceneLights.selectedType == i); 
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
			{
				scene.sceneLights.selectedType = i;
				k = i;
			}
		}
		ImGui::Text(std::to_string(scene.sceneLights.selectedType).c_str());
		ImGui::ColorEdit3("LightColor", &scene.sceneLights.defaultColors[0]);
		ImGui::DragFloat3("LightPosition", &scene.sceneLights.Position[0]);
		ImGui::DragFloat("Strength", &scene.sceneLights.deafaultStrength);
		if (ImGui::Button("addLight")) {

			scene.sceneLights.addLights(scene.objects[scene.selectionIndex].position, scene.sceneLights.defaultColors, scene.sceneLights.deafaultStrength,static_cast<Type> (scene.sceneLights.selectedType));
			scene.sceneLights.panelStatus = false;
		}
		//ImGui::Text(std::to_string(count).c_str());
		ImGui::End();

	}
	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
	{

		if (uMsg == BFFM_INITIALIZED)
		{
			std::string tmp = (const char*)lpData;
			std::cout << "path: " << tmp << std::endl;
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		}

		return 0;
	}
	string SaveFile(const char *filter) {
		OPENFILENAME ofn;
		TCHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		HWND hWnd = glfwGetWin32Window(window);
		ofn.hwndOwner = hWnd;
		ofn.lpstrFilter = filter;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_EXPLORER;
		if (GetSaveFileName(&ofn) == TRUE) {
			return string(ofn.lpstrFile);
		}
		return "";
	}

	bool LoadFile(const char *filter,vector<std::string> &path) {
		OPENFILENAME ofn;       // common dialog box structure
		TCHAR szFile[260] = { 0 };       // if using TCHAR macros

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);

		HWND hWnd = glfwGetWin32Window(window);
		ofn.hwndOwner = hWnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		//ofn.lpstrFilter = _T("Sav Files (*.sav)\0*.sav\0");
		//const char *a = _T("Sav Files (*.sav)\0*.sav\0");
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER;
		//ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER;

		if (GetOpenFileName(&ofn) == TRUE)
		{
			const char* p = ofn.lpstrFile;
		
			char* str = ofn.lpstrFile;
			std::string directory = str;
			str += (directory.length() + 1);
			string fileDirectory = ofn.lpstrFile;
			vector<string> fileList;
			while (*str) {
				std::string filename = str;
				str += (filename.length() + 1);
				fileList.push_back(fileDirectory + "\\" + str);
			}
			if (fileList.size() > 0) {
				path = fileList;

			}
			else {
				path.push_back((ofn.lpstrFile));
			}
		   
			// string str = static_cast<string>(ofn.lpstrFile);
			 

			//std::cout << str.length() << "\n";
			return true;
		}
		return false;
	}

	std::string BrowseFolder(std::string saved_path)
	{
		TCHAR path[MAX_PATH];

		const char* path_param = saved_path.c_str();

		BROWSEINFO bi = { 0 };
		bi.lpszTitle = ("Choose animation saving folder");
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
		bi.lpfn = BrowseCallbackProc;

		bi.lParam = (LPARAM)path_param;

		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

		if (pidl != 0)
		{
			//get the name of the folder and put it in path
			SHGetPathFromIDList(pidl, path);

			//free memory used
			IMalloc* imalloc = 0;
			if (SUCCEEDED(SHGetMalloc(&imalloc)))
			{
				imalloc->Free(pidl);
				imalloc->Release();
			}

			return path;
		}

		return "";
	}

	void BrowseSave(Scene &scene) {
		std::string finalFolderPath = BrowseFolder("D:\\newSaveFolder\\newSave3");
		scene.saveSceneData(finalFolderPath);
		//std::cout << finalFolderPath << "\n";

	}
	void loadScene(Scene &scene) {
	
	//	ImGui::SetCursorPos(ImVec2(0, 100));

		
		
			const char *filter = _T("Sav Files (*.sav)\0*.sav\0");

			vector<std::string> path;
			if(LoadFile(filter,path)){
			scene.loadSceneData(path[0]);
		}
		


		
	}



	void loadTexture(Scene& scene) {
	
		if(ImGui::Button("Load Texture")) {
			const char* filter = _T("Image Files (*.png, *.jpg,*.jpeg, *.hdr, *.tga)\0*.png;*.jpg;*.jpeg;*.tga;*.hdr\0");
			vector<std::string> path;
			if (LoadFile(filter, path)) {
				for (auto file : path) {
					scene.loadTexture(file);
				}
				
			}
		}

		for (Texture& t : scene.textures) {
			string w = std::to_string(t.width);
			static bool change = false;
			static bool first = false;
			if (change) {
			
				
				//t.name.resize(100);
				string* name = &t.name;
				ImGui::InputText("setTheName", (char*)name->c_str(), name->capacity() + 1);
			}
				
			
			else {
				ImGui::Selectable(t.name.c_str(), false); //ImGui::SameLine(); ///ImGui::Text(w.c_str()); 
			}
			
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) { 
				change = true;
			}
			else if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
				change = false;
			}
			

		}

		
	}
	

	void objectTexture(Scene &scene) {
	//	ImGui::Begin("textures");
		if (scene.textures.size() > 0) {
			Objects *object = &scene.objects[scene.selectionIndex];
			if (object->textureID.size() < scene.textures.size()) {
			if(ImGui::Button("Add texture")) {
				
				object->textureID.push_back(0);

			}
			}
			//ImGuiStyle& style = ImGui::GetStyle();
			if (object->textureID.size() > 0) {

				for (int t = 0; t < object->textureID.size(); t++) {
			
		
					if (ImGui::BeginCombo(std::to_string(t).c_str(), scene.textures[scene.objects[scene.selectionIndex].textureID[t]].name.c_str()))
					{
						for (int n = 0; n < scene.textures.size(); n++)
						{
							bool is_selected = (scene.objects[scene.selectionIndex].textureID[t] == n);
							if (ImGui::Selectable(scene.textures[n].name.c_str(), is_selected)) {
								scene.objects[scene.selectionIndex].textureID[t] = n;
					
							}
					
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
						//ImGui::Text("separe");
						/*
						ImVec2 pos = ImGui::GetCursorScreenPos();
						ImVec2 maxPos = ImVec2(pos.x + ImGui::GetWindowSize().x, pos.y + ImGui::GetWindowSize().y);
						unsigned texId = scene.tex2;
						ImGui::GetWindowDrawList()->AddImage(
							(void*)texId,
							ImVec2(pos.x, pos.y),
							ImVec2(maxPos),
							ImVec2(0, 0), ImVec2(1, 1)
						);
						*/
					}
				}
			}
		}
	}

	void ObjectMaterial(Scene& scene) {

		const char *names[3] = {"Default","Textured","PBR"};
		

		if (ImGui::BeginCombo("Choose Materials", names[scene.objects[scene.selectionIndex].mat.mode]))
		{
			for (int n = 0; n < 3; n++)
			{
				bool is_selected = (scene.objects[scene.selectionIndex].mat.mode == n);
				if (ImGui::Selectable(names[n], is_selected)) {
					//scene.objects[scene.selectionIndex].textureID[0] = n;
					float p = 0;
					scene.objects[scene.selectionIndex].mat.mode = static_cast<Material::Mode>(n);
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	

	}

	void DiffuseTexture(Scene& scene) {
		
		Objects *object = &scene.objects[scene.selectionIndex];
		if (object->textureID.size() > 0) {

			if (ImGui::BeginCombo("Choose Diffuse Texture", scene.textures[object->textureID[object->mat.DiffuseTextureID]].name.c_str()))
			{
				for (int n = 0; n < object->textureID.size(); n++)
				{
					bool is_selected = (object->mat.DiffuseTextureID == n);
					
					if (ImGui::Selectable(scene.textures[object->textureID[n]].name.c_str(), is_selected)) {
						object->mat.DiffuseTextureID = n;

					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}

	}


	void BloomTexture(Scene& scene) {

		Objects* object = &scene.objects[scene.selectionIndex];
		if (object->textureID.size() > 0) {

			if (ImGui::BeginCombo("Choose Bloom Texture", scene.textures[object->textureID[object->mat.bloomTextureID]].name.c_str()))
			{
				for (int n = 0; n < object->textureID.size(); n++)
				{
					bool is_selected = (object->mat.bloomTextureID == n);

					if (ImGui::Selectable(scene.textures[object->textureID[n]].name.c_str(), is_selected)) {
						object->mat.bloomTextureID = n;

					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}

	}


	void GuiObjectTextureSelector(Scene& scene,string name,unsigned int &ID){

		Objects* object = &scene.objects[scene.selectionIndex];
		string finalTitle = "choose " + name + " Texture";
		if (object->textureID.size() > 0) {

			if (ImGui::BeginCombo(finalTitle.c_str(), scene.textures[object->textureID[ID]].name.c_str()))
			{
				for (int n = 0; n < object->textureID.size(); n++)
				{
					bool is_selected = (ID == n);

					if (ImGui::Selectable(scene.textures[object->textureID[n]].name.c_str(), is_selected)) {
						ID = n;

					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
	
	}
	void AnimationActionSelector(Animation *animation,Objects *object) {
		string finalTitle = "choose actions";
		if (ImGui::BeginCombo(finalTitle.c_str(), animation->actions [object->actionIndex].name.c_str())) {
			for (int i = 0; i < animation->actions.size(); i++) {
				bool is_selected = (object->actionIndex == i);
				if (ImGui::Selectable(animation->actions[i].name.c_str(), is_selected)) {
					object->actionIndex = i;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
				
			}
			ImGui::EndCombo();
		}
	}
	void BlendActionSelector(Animation* animation, Objects* object) {
		string finalTitle = "choose Blend action";
		if (ImGui::BeginCombo(finalTitle.c_str(), animation->actions[object->blendIndex].name.c_str())) {
			for (int i = 0; i < animation->actions.size(); i++) {
				bool is_selected = (object->blendIndex == i);
				if (ImGui::Selectable(animation->actions[i].name.c_str(), is_selected)) {
					object->blendIndex = i;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}

			}
			ImGui::EndCombo();
		}
	}

	void EnvironmentMapGenerator(Scene& scene) {
		//if(ImGui::BeginCombo("CubeMapGenerator",))
		static unsigned int a;
		if (scene.textures.size() > 0) {
			if (ImGui::BeginCombo("Choose Texture", scene.textures[a].name.c_str())) {
				for (int n = 0; n < scene.textures.size(); n++) {
					bool is_selected = (a == n);
					if (ImGui::Selectable(scene.textures[n].name.c_str(), is_selected)) {
						a = n;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();

			}
			//ImGui::SameLine();
			if (ImGui::Button("Generate")) {
				scene.environmentMapGenerator(a);

			}
		}
		
		

	}
	void chooseTexture(Scene& scene, static unsigned int &index) {
		if (scene.textures.size() > 0) {
			if (ImGui::BeginCombo("Choose Texture", scene.textures[index].name.c_str())) {
				for (int n = 0; n < scene.textures.size(); n++) {
					bool is_selected = (index == n);
					if (ImGui::Selectable(scene.textures[n].name.c_str(), is_selected)) {
						index = n;
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();

			}
		}
	}
	void draw(Scene& scene) {

		//GUI Theme
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
		colors[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
		colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
		colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
		colors[ImGuiCol_TabActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	 
		colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

		style->ChildRounding = 4.0f;
		style->FrameBorderSize = 1.0f;
		style->FrameRounding = 2.0f;
		style->GrabMinSize = 7.0f;
		style->PopupRounding = 2.0f;
		style->ScrollbarRounding = 12.0f;
		style->ScrollbarSize = 13.0f;
		style->TabBorderSize = 1.0f;
		style->TabRounding = 0.0f;
		style->WindowRounding = 4.0f;


		//Object ProperTies panel
		{
	
			
			ImGui::Begin("Properties");
			ImGui::BeginTabBar("Transformation",1);
			if (ImGui::BeginTabItem("Transformation")) {
				if (scene.objects.size() > 0) {
					ImGui::DragFloat3("position", &scene.objects[scene.selectionIndex].position[0], 0.1, 0.0, 0.0, "%3f");
					ImGui::DragFloat3("rotation", &scene.objects[scene.selectionIndex].rotation[0], 0.1, 0.0, 0.0, "%3f");
					ImGui::DragFloat3("scale", &scene.objects[scene.selectionIndex].scale[0], 0.1, 0.0, 0.0, "%3f");
				}
				ImGui::EndTabItem();

			}
			if (ImGui::BeginTabItem("Textures")) {
				if (scene.objects.size() > 0) {
					objectTexture(scene);
				}
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Materials")) {
				
				if (scene.objects.size() > 0)
				{ 
					ObjectMaterial(scene);
					if (scene.objects[scene.selectionIndex].mat.mode == Material::Default) {
						ImGui::ColorEdit4("ObjectColor", &scene.objects[scene.selectionIndex].mat.objectColor[0]);
					}
					else if (scene.objects[scene.selectionIndex].mat.mode == Material::Textured) {
						DiffuseTexture(scene);

					}
					else if (scene.objects[scene.selectionIndex].mat.mode == Material::PBR) {
						DiffuseTexture(scene);
						GuiObjectTextureSelector(scene, "normalMap", scene.objects[scene.selectionIndex].mat.NormalTextureID);
						GuiObjectTextureSelector(scene, "Glossy", scene.objects[scene.selectionIndex].mat.GlossyTextureID);
						GuiObjectTextureSelector(scene, "Roughness", scene.objects[scene.selectionIndex].mat.RoughnessTextureID);
						GuiObjectTextureSelector(scene, "AmbientMap", scene.objects[scene.selectionIndex].mat.AmbientTextureID);
						GuiObjectTextureSelector(scene, "PreFilter", scene.objects[scene.selectionIndex].mat.environmentTexture);
						GuiObjectTextureSelector(scene, "Irradiance", scene.objects[scene.selectionIndex].mat.irradianceTexture);
						GuiObjectTextureSelector(scene, "BrdfTexture", scene.objects[scene.selectionIndex].mat.brdfTexture);
						ImGui::DragFloat("Roughness", &scene.objects[scene.selectionIndex].mat.roughness, 0.01, 0.0, 5.0);
						ImGui::DragFloat("Glossiness", &scene.objects[scene.selectionIndex].mat.metalness, 0.01, 0.0, 1.0);
						ImGui::DragFloat("Ambient", &scene.objects[scene.selectionIndex].mat.ambient, 0.01, 0.0, 1.0);
					}
					
					if (ImGui::CollapsingHeader("Bloom", 0)) {
						ImGui::Checkbox("Enable Bloom", &scene.objects[scene.selectionIndex].mat.bloom);
						if (scene.objects[scene.selectionIndex].mat.bloom) {
							ImGui::Checkbox("Set Bloom Texture", &scene.objects[scene.selectionIndex].mat.bloomTexture);
							if (scene.objects[scene.selectionIndex].mat.bloomTexture) {
								BloomTexture(scene);
							}
						}
						
						
						//ImGui::ColorEdit3("bloom color", &scene.objects[scene.selectionIndex].bloomColor[0]);
					}
					if (ImGui::CollapsingHeader("Blending", 0)) {
						ImGui::Checkbox("Enable Blending", &scene.objects[scene.selectionIndex].mat.blending);
					}
					
					
				}
				
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Animation")) {
				//Mesh *mesh = &scene.allPrimitives[scene.objects[scene.selectionIndex].primitiveID].mesh;
				if(scene.objects.size()>0){
					Objects* ob = &scene.objects[scene.selectionIndex];
					if(ob->animationIDs.size()>0){
						if (ImGui::CollapsingHeader("Animation Actions")) {
							

							
							Animation* anim = &scene.animations[ob->animationIDs[0]];
							float k = 0.0f;
							float n = 12.0f;
							//ImGui::DragFloatRange2("AnimationTime", &k, &n, 1	.0);
							AnimationActionSelector(anim,ob);
							if (ob->animPlay) {
								if (ImGui::Button("pause")) {
									ob->animPlay = false;
								}
							}
							else {
								if (ImGui::Button("play")) {
									ob->animPlay = true;
								}

							}
							float range = anim->actions[ob->actionIndex].range.y - anim->actions[ob->actionIndex].range.x;
							ImGui::SliderFloat(anim->actions[ob->actionIndex].name.c_str(), &ob->animationTime,0.0, range);
							//ImGui::DragFloat("Animation Speed", &anim->actions[ob->actionIndex].speed);
			
							ImGui::SliderFloat("blend factor", &ob->blendFactor, 0.0, 1.0);
							BlendActionSelector(anim, ob);
							if (ImGui::CollapsingHeader("Action List")) {
								for (unsigned int i = 0; i < anim->actions.size(); i++) {
									ImGui::DragFloat((std::string("Action_")+anim->actions[i].name).c_str(), &anim->actions[i].range.x,1.0,0.0,anim->duration*anim->fps);

									//ImGui::DragFloatRange2(anim->actions[i].name.c_str(), &anim->actions[i].range.x,)
								}
							}
						}
					}
				}
			ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
			ImGui::End();

		}

		



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




		//Scene Properties panel

		{

			ImGui::Begin("Scene Properties");
			ImGui::BeginTabBar("Scene Properties", 1);
			if (ImGui::BeginTabItem("Scene Color")) {
				ImGui::ColorEdit4("clear color", &scene.ClearColor[0]);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Render")) {
				ImGui::Checkbox("Display Only Render",&scene.onlyRender);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Tools")) {
				if(ImGui::CollapsingHeader("EnvironmentMap Generator")) {
					EnvironmentMapGenerator(scene);

				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
			ImGui::End();

		}






/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Scene object list panel



		ImGui::Begin("List");
		ImGui::BeginTabBar("List", 1);
		if (ImGui::BeginTabItem("Object List")) {
			{
				//ImGui::Begin("Objects_List");
				int l = 0;
				for (Objects& object : scene.objects) {


					ImGui::Selectable(object.name.c_str(), object.selected);
					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
					{
						if (scene.previous != l && scene.previous > -1) {
							scene.objects[scene.previous].selected = false;
						}
						object.selected = !object.selected;
						Camera* cam = scene.camera;
						cam->objectTrackPosition = object.position;
						cam->objectTacking = true;

						scene.selectionIndex = l;
						scene.previous = l;

					}

					l += 1;
				}
				
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Mesh List")) {
			{
				for (Primitives& p : scene.allPrimitives) {
					ImGui::Selectable(p.name.c_str(), false);
				}

			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Animation List")) {
			{
				for (Animation& a : scene.animations) {
					static bool change = false;
					if (change) {
						//	a.name.resize(100);
						string* name = &a.name;
						//ImGui::InputText("setTheName", (char*)name->c_str(), name->capacity() + 1);
						ImGui::InputText("Name", name);
					}
					else {
						ImGui::Selectable(a.name.c_str(), a.selected);
					}
					
					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
					{
						// Do stuff on Selectable() double click.                                                                                                                                                                                                                           
						a.selected = !a.selected;
					}

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
						change = true;
					}
					else if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
						change = false;
					}
				}

			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Light List")) {
			{
				for (Light& light : scene.sceneLights.lights) {
				ImGui::Selectable(light.profile.name.c_str(), scene.sceneLights.selectedIndex == light.index);
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
				{
					scene.sceneLights.selectedIndex = light.index;
				}
			}

			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Texture List")) {
			{
				loadTexture(scene);

			}
			ImGui::EndTabItem();
		}


		ImGui::EndTabBar();

		ImGui::End();





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




		{

			static bool my_tool_active = true;
			
			ImGui::Begin("File", &my_tool_active, ImGuiWindowFlags_MenuBar);
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Open..", "Ctrl+O")) {
						loadScene(scene);

					}
					if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ 
						
						const char* filter = _T("Sav Files (*.sav)\0*.sav\0");
						string savefilePath = SaveFile(filter);
						if (savefilePath!="") {
							string ext = Tools::getExtensionName(savefilePath);
							if (ext != "sav") {
								savefilePath += ".sav";
							}
							scene.saveSceneData(savefilePath);
						}
						
					}
					
					if (ImGui::MenuItem("Export", "Ctrl+E")) { /* Do stuff */

						const char* filter = _T("Sav Files (*.sav)\0*.sav\0");
						string savefilePath = SaveFile(filter);

						if (savefilePath != "") {
							string ext = Tools::getExtensionName(savefilePath);
							if (ext != "sav") {
								savefilePath += ".sav";
							}
							scene.saveSceneData(savefilePath,true);
						}

					}

					if (ImGui::MenuItem("Save Js Primitve", "Ctrl+J")) { /* Do stuff */

						const char* filter = _T("Sav Files (*.js)\0*.js\0");
						string savefilePath = SaveFile(filter);

						if (savefilePath != "") {
							string ext = Tools::getExtensionName(savefilePath);
							if (ext != "js") {
								savefilePath += ".js";
							}
							scene.saveJsScene(savefilePath);
						}

					}
					if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; 
					glfwSetWindowShouldClose(window, GL_TRUE);
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
				
				
			}
			

				
			{
			
				 
				 static unsigned int index = 0;
				 chooseTexture(scene, index);
				 if (ImGui::Button("saveSaved")) {
					 scene.save();
				 }
				 if (ImGui::Button("Save Tex")) {
					// string path = BrowseFolder("");
					 const char* filter = _T("Image Files (*.png, *.jpg,*.jpeg, *.hdr, *.tga)\0*.png;*.jpg;*.jpeg;*.tga;*.hdr\0");
					 string finalPath = SaveFile(filter);
					 scene.saveTexture(index, finalPath);
					// CopyFile()

				 }
			
			}
			
			

			ImGui::End();
		
		
		}
		



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




		if (scene.sceneLights.panelStatus) {
			openLightPanel(scene);
		}

		
	}
	void render() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	void terminate() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}
};