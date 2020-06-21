enum Type{Point,Directional,Spot};

struct Profile{
	string name;
	Type type;
};

struct Light {
public:
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 ambientColor = glm::vec3(1.0);
	float strength;
	Profile profile;
	int index;

	Light(Profile pro, glm::vec3 pos = glm::vec3(0.0), glm::vec3 col = glm::vec3(1.), float lightStrength = 1.0) {
		profile = pro;
		position = pos;
		color = col;
		strength = lightStrength;
	}

	
};

class Lights {
public:
	std::vector<Light> lights;
	int selectedIndex = 0;
	bool panelStatus = 0;
	int selectedType = 0;
	glm::vec3 defaultColors = glm::vec3(0.0);
	float deafaultStrength = 1.0;
	void addLights( glm::vec3 pos, glm::vec3 col, float lightStrength,Type type = Point) {
		Profile pro;
		type = static_cast<Type>(selectedType);
		pro.type = type;
		string lightName[3] = {"(Point)","(Directional)" ,"(Spot)" };
		pro.name = "Light" + std::to_string(lights.size())+"   "+lightName[type];
		Light  light = Light(pro, pos, col, lightStrength);
		light.index = lights.size();
		lights.push_back(light);
	}

};
