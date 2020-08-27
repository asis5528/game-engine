#version 330 core
			layout (location = 0) in vec3 aPos;
	uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float offset;

vec2 pos[2] = vec2[2](vec2(0.,0.),vec2(1.,1.));

			void main()
			{

			//gl_VertexID
			 gl_Position =projection*view*model* vec4(aPos.x, aPos.y, aPos.z, 1.0);
		//	gl_Position = vec4(pos[gl_VertexID],0.,1.);
			}