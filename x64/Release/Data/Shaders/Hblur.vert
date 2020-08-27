#version 330 core
			layout (location = 0) in vec3 aPos;
			layout (location = 1) in vec2 aUV;
			out vec2 uv;
			out vec2 blurTexCoords[11];
			uniform float width;
			void main()
			{
			 gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
			 uv = vec2(aUV.x,aUV.y);
			 float pixelSize = 1./(1200/4);
		for(int i=-5;i<5;i++){
			blurTexCoords[i+5]=aUV+vec2(pixelSize*float(i),0.0);
		}
			}