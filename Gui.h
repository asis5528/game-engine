#include <Shlwapi.h>
#include <shlobj.h> 

class Gui {
GLFWwindow *window;
public:
	int count = 0;
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
		ImGui::DragFloat("Strength", &scene.sceneLights.deafaultStrength);
		if (ImGui::Button("addLight")) {

			scene.sceneLights.addLights(scene.camera->objectTrackPosition, scene.sceneLights.defaultColors, static_cast<Type> (scene.sceneLights.selectedType));
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
		std::string finalFolderPath = BrowseFolder("");
		scene.saveSceneData(finalFolderPath);
		//std::cout << finalFolderPath << "\n";

	}
	void loadScene(Scene &scene) {
		ImGui::Begin("Loader");
		ImGui::SetCursorPos(ImVec2(0, 100));
		static char elevenBytes[15] = "firstModel";
		
		ImGui::InputText("setTheName", elevenBytes, sizeof(elevenBytes));
		
		if (ImGui::Button("Load")) {
			//const char path = reinterpret_cast<char> (elevenBytes));
			OPENFILENAME ofn;       // common dialog box structure
			TCHAR szFile[260] = { 0 };       // if using TCHAR macros

			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);

			HWND hWnd = glfwGetWin32Window(window);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			//ofn.lpstrFilter = _T("All\0*.*\0Text\0*.TXT\0");
			ofn.nFilterIndex = 1;
			
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			//ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				string a = static_cast<string>(ofn.lpstrFile);
				std::cout << a << "\n";
				scene.loadSceneData(a);
				// use ofn.lpstrFile
			}

			//string n = elevenBytes;
			//scene.loadScene(n);
		}
		
		
		//if (ImGui::Button("Load")) {
		//	scene.loadSceneData();
		//}
		ImGui::End();
		
	}

	void draw(Scene& scene) {
		loadScene(scene);
		{

			ImGui::Begin("TRANSFORMATION");
			if (scene.objects.size() > 0) {
				ImGui::DragFloat3("position", &scene.objects[scene.selectionIndex].position[0], 0.1, 0.0, 0.0, "%3f");
				ImGui::DragFloat3("rotation", &scene.objects[scene.selectionIndex].rotation[0], 0.1, 0.0, 0.0, "%3f");
				ImGui::DragFloat3("scale", &scene.objects[scene.selectionIndex].scale[0], 0.1, 0.0, 0.0, "%3f");
			}
			ImGui::End();

		}

		

		{

			ImGui::Begin("color");
			ImGui::ColorPicker3("clear color", &scene.ClearColor[0]);
			ImGui::End();

		}

		{
			ImGui::Begin("mesh List"); 
			for (Primitives& p : scene.allPrimitives) {
				ImGui::Selectable(p.name.c_str(), false);
			}
			ImGui::End();


		}
		{
			ImGui::Begin("save primitives");
			if (ImGui::Button("save")) {
				//scene.savePrimitive();
				//

		}
			if (ImGui::Button("Save AS")) {
				//scene.savePrimitive();
				 BrowseSave(scene);

				//scene.saveSceneData();

			}
			ImGui::End();


		}
		{
			ImGui::Begin("Objects_List");
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
			ImGui::End();
		}
		if (scene.sceneLights.panelStatus) {
			openLightPanel(scene);
		}

		{
			ImGui::Begin("Lights List");
			for (Light& light : scene.sceneLights.lights) {
				ImGui::Selectable(light.profile.name.c_str(),scene.sceneLights.selectedIndex==light.index);
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
				{
					scene.sceneLights.selectedIndex = light.index;
				}
			}
			ImGui::End();

		}

		//}


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