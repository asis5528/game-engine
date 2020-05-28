class Gui {
public:	
	void init(GLFWwindow *window) {
		ImGui::CreateContext();
		//ImGui::StyleColorsDark();
		const char* glsl_version = "#version 330";
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
	}
	void start() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	void draw(Scene &scene) {
		{
			
			ImGui::Begin("TRANSFORMATION");
			if (scene.objects.size() > 0) {
				ImGui::DragFloat3("position", &scene.objects[scene.selectionIndex].position[0], 0.1, 0.0, 0.0, "%3f");
				ImGui::DragFloat3("rotation", &scene.objects[scene.selectionIndex].rotation[0], 0.1, 0.0, 0.0, "%3f");
				ImGui::DragFloat3("scale", &scene.objects[scene.selectionIndex].scale[0], 0.1, 0.0, 0.0, "%3f");
				//static char str0[128] = "Hello, world!";
				//ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));
				//ImGui::DragFloat("FOV", &scene.zoom);
			}
			//char buf[15] = { "hellow" }; ImGui::InputText("Name", buf, IM_ARRAYSIZE(buf));
			ImGui::End();

		}



		{
			
			ImGui::Begin("color");
			ImGui::ColorPicker3("clear color", &scene.ClearColor[0]);
			ImGui::End();

		}
		{
			ImGui::Begin("Objects_List");
			static const char* it[5]{ "terrain","tree","enderMan","Sky","tank" };





			int l = 0;
			






			for (Objects& object : scene.objects) {


				ImGui::Selectable(object.name.c_str(), object.selected);
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
				{
					//Objects Duplicate = Objects(object.model, object.shader, object.name+std::to_string(l));
					//objects.push_back(Duplicate);
					// Do stuff on Selectable() double click.      
					if (scene.previous != l && scene.previous > -1) {
						scene.objects[scene.previous].selected = false;
					}
					object.selected = !object.selected;
					Camera *cam = scene.camera;
					cam->objectTrackPosition = object.position;
					cam->objectTacking = true;
				
					scene.selectionIndex = l;
					scene.previous = l;

				}


				l += 1;
			}



			ImGui::End();
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