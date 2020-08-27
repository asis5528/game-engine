
  
uniform sampler2D colorSampler;  // The color, rendered from the default Step 0
uniform sampler2D depthSampler;  // The depth, rendered from the default Step 0
uniform sampler2D normalSampler; // The normals, rendered from "prep-step"

varying vec2 uv; // The texture coordinate the quad
varying vec3 viewSpace;
float scale = 0.5;
vec4 _Color = vec4(1.,0.,0.,1.);

float _DepthThreshold = 0.5;
float _DepthNormalThreshold = 0.5;
float _DepthNormalThresholdScale = 0.5;
float _NormalThreshold = 0.5;
vec2 pixelSize = vec2(1./1024.,1./1024.);
//mat4 clipSpace;

vec4 alphaBlend(vec4 top, vec4 bottom)
            {
                vec3 color = (top.rgb * top.a) + (bottom.rgb * (1. - top.a));
                float alpha = top.a + bottom.a * (1. - top.a);

                return vec4(color, alpha);
            }

void main(){
    
    float halfScaleFloor = floor(scale*0.5);
    float halfScaleCeil = ceil(scale*0.5);
    
    vec2 BLUV = uv-pixelSize*halfScaleFloor;
    vec2 TRUV = uv+pixelSize*halfScaleCeil;
    vec2 BRUV = uv+vec2(pixelSize.x*halfScaleCeil,-pixelSize.y*halfScaleFloor);
    vec2 TLUV = uv+vec2(-pixelSize.x*halfScaleFloor,pixelSize.y*halfScaleCeil);
    
    vec3 norm0 = texture2D(normalSampler,BLUV).rgb;
    vec3 norm1 = texture2D(normalSampler,TRUV).rgb;
    vec3 norm2 = texture2D(normalSampler,BRUV).rgb;
    vec3 norm3 = texture2D(normalSampler,TLUV).rgb;
    
    float dep0 = texture2D(depthSampler,BLUV).r;
    float dep1 = texture2D(depthSampler,TRUV).r;
    float dep2 = texture2D(depthSampler,BRUV).r;
    float dep3 = texture2D(depthSampler,TLUV).r;
    
    vec3 viewNormal = norm0*2.-1.;
    float NdotV = 1.-dot(viewNormal,-viewSpace);
    
    float normalThreshold01 = clamp((NdotV - _DepthNormalThreshold) / (1. - _DepthNormalThreshold),0.,1.);
    float normalThreshold = normalThreshold01*_DepthNormalThresholdScale+1.;

    float depthThreshold = _DepthThreshold*dep0*normalThreshold;

    float depthFiniteDifference0 = dep1-dep0;
    float depthFiniteDifference1 = dep3 - dep2;
    float edgeDepth = sqrt(pow(depthFiniteDifference0, 2.) + pow(depthFiniteDifference1, 2.)) * 100.;
    edgeDepth = edgeDepth > depthThreshold ? 1. : 0.;
    vec3 normalFiniteDifference0 =norm1 - norm0;
    vec3 normalFiniteDifference1 = norm3 - norm2;
    float edgeNormal = sqrt(dot(normalFiniteDifference0, normalFiniteDifference0) + dot(normalFiniteDifference1, normalFiniteDifference1));
    edgeNormal = edgeNormal > _NormalThreshold ? 1. : 0.;
    float edge = max(edgeDepth, edgeNormal);

    vec4 edgeColor = vec4(_Color.rgb, _Color.a * edge);
    vec4 color = texture2D(colorSampler,uv);
    gl_FragColor=  alphaBlend(color,edgeColor);
}

