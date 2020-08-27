#version 330 core
out vec4 FragColor;
in vec2 uv;
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform float timer;
uniform float width;
uniform bool bloom;


float lineSegment(vec2 p, vec2 a, vec2 b) {
    vec2 pa = p - a, ba = b - a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return clamp(1.-smoothstep(0.0, 5.2 /width, length(pa - ba*h)/22.),0.,1.);
}

			void main()
{
			 
			 vec2 st = vec2(uv.x,1.-uv.y);
			
			 vec4 offsetCol =texture(tex1,vec2(clamp(st.x+0.003,0.,1.),st.y));
			 offsetCol +=texture(tex1,vec2(clamp(st.x-0.003,0.,1.),st.y));
			 offsetCol +=texture(tex1,vec2(st.x,clamp(st.y+0.003,0.,1.)));
			 offsetCol +=texture(tex1,vec2(st.x,clamp(st.y-0.003,0.,1.)));
			 vec4 col = texture(tex1,vec2(st.x,st.y));
			FragColor =clamp(clamp(offsetCol,0.,1.)-col,0.,1.);
			FragColor = mix(texture(tex0,st),FragColor*vec4(1.,0.3,0.5,1.),FragColor.x);
			FragColor.a = 1.0;
			if(bloom){
			FragColor += texture(tex2,st);
			}


		
			}