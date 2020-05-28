#version 330 core
out vec4 FragColor;
in vec2 uv;
uniform sampler2D tex;
uniform float timer;


			void main()
{
			 if(timer>5.0)discard;
			 FragColor =texture(tex,vec2(uv))*(sin(timer*4.0)*0.5+0.5);

		
			}