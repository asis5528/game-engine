#version 330 core
out vec4 FragColor;
in vec2 uv;
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform float timer;


			void main()
{/*
			 if(timer>5.0)discard;
			 FragColor =texture(tex,vec2(uv))*(sin(timer*4.0)*0.5+0.5);
			 */
			 
			 vec2 st = vec2(uv.x,1.-uv.y);
			 vec4 offsetCol =texture(tex1,vec2(clamp(st.x+0.003,0.,1.),st.y));
			 offsetCol +=texture(tex1,vec2(clamp(st.x-0.003,0.,1.),st.y));
			 offsetCol +=texture(tex1,vec2(st.x,clamp(st.y+0.003,0.,1.)));
			 offsetCol +=texture(tex1,vec2(st.x,clamp(st.y-0.003,0.,1.)));
			 vec4 col = texture(tex1,vec2(st.x,st.y));
		FragColor =clamp(offsetCol,0.,1.)-col;
		FragColor += texture(tex0,st);
		//FragColor = mix(texture(tex2,vec2(uv))*(sin(timer*4.0)*0.5+0.5),FragColor,smoothstep(5.,6.,timer));
		//FragColor*=col.r;
		if(FragColor.a<0.3){
		}else{
		//FragColor =vec4(0.);
		}
			}