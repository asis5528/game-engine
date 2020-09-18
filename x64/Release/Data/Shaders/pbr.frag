#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Color;
in vec2 TexCoords;
in vec3 normal;
in vec3 modelMatPos;

uniform bool bloom;
uniform bool BloomTexture;


uniform sampler2D DiffuseTexture;
uniform sampler2D normalMap;
uniform sampler2D glossyTexture;
uniform sampler2D roughnessTexture;
uniform sampler2D GlowTexture;
uniform sampler2D AoTexture;

uniform samplerCube envMap;
uniform samplerCube IrradianceTexture;
uniform sampler2D brdfTexture;


uniform vec4 objectColor;
uniform int selected;
uniform vec3 camPos;

uniform float roughness;
uniform float metalness;
uniform float Ambient;


uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform int LightSize;

const float PI = 3.14159265359;



vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(modelMatPos);
    vec3 Q2  = dFdy(modelMatPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}




void main()
{  

	vec3 N = getNormalFromMap();
	//vec3 N = normalize(normal);
	vec3 V = normalize(camPos-modelMatPos);
	vec3 R = reflect(-V,normalize(N));

	vec4 diffuseTexture = texture(DiffuseTexture,TexCoords);
//	diffuseTexture=vec4(0.2,0.1,1.,1.);
	float metallic = texture(glossyTexture,TexCoords).r*metalness;
	float roughnessVal = texture(roughnessTexture, TexCoords).r*roughness;
	float ao  = texture(AoTexture,TexCoords).r*Ambient;
	if(Ambient<0.1){
	ao = 1.;
	}
	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, diffuseTexture.rgb, metallic);
	float fakeSSS = pow(smoothstep(0.5,1.0,1.0-dot(N,V)),1.25);
	vec3 Lo = vec3(0.0);
    for(int i = 0; i < LightSize; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - modelMatPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - modelMatPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughnessVal);   
        float G   = GeometrySmith(N, V, L, roughnessVal);    
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;
        
         // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	                
            
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * texture(IrradianceTexture, R).rgb* diffuseTexture.rgb / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   


	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughnessVal);
	 vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(IrradianceTexture, N).rgb;
    vec3 diffuse   = irradiance * diffuseTexture.rgb;
	
	const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(envMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfTexture, vec2(max(dot(N, V), 0.0), roughnessVal)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

	vec3 d = vec3(1.0);
	vec3 fi = mix(d,prefilteredColor,1.);
	vec3 ambient = (kD * diffuse + specular+vec3(fakeSSS)*0.0)*ao;
	 vec3 color = ambient+Lo;
	 color = color / (color + vec3(1.0));
    //gamma correct
    color = pow(color, vec3(1.0/2.2)); 
	FragColor = vec4(color,1.);
	//FragColor =  textureLod(envMap, R,  roughness * MAX_REFLECTION_LOD);    
//	FragColor = vec4(fi,1.0);
	//FragColor = vec4(roughnessVal,roughnessVal,roughnessVal,1.);
	//FragColor = texture(AoTexture,TexCoords);
	//FragColor = vec4(lightColors[LightSize],1.);
	//FragColor =vec4(diffuse,1.)+texture(IrradianceTexture, R,roughnessVal * MAX_REFLECTION_LOD)*metalness;
	if(bloom){
		Color = FragColor;

		if(BloomTexture){
			vec4 glow =  texture(GlowTexture,TexCoords);
	
			Color = glow*vec4(15.);
		}
	}
	else{
		Color = vec4(0.,0.,0.,1.);
	}

	if(selected == 1){
		FragColor = vec4(1.0,1.0,1.,1.);
	}
	
}