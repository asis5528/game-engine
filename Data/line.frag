#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 col;
uniform vec3 color;
void main()
{
col = vec4(-1.);
			FragColor = vec4(color*0.7,1.);
		
			}