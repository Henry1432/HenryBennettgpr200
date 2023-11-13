#version 450
out vec4 FragColor;

in Surface{
	vec2 UV; //Per-fragment interpolated UV
	vec3 WorldPosition; //Per-fragment interpolated world position
	vec3 WorldNormal; //Per-fragment interpolated world normal
}fs_in;

struct Light
{
	vec3 position;
	vec3 color;
};
#define MAX_LIGHTS 4
uniform Light _Lights[MAX_LIGHTS];

uniform Light _Light;

uniform sampler2D _Texture;

uniform float _AmbientK = 0.3;
uniform float _DiffuseK = 0.7;

float calcLight(vec3 normal){
	return _AmbientK + _DiffuseK * max(dot(-_Light.position,normal),0.0);
}

void main(){
	vec3 normal = normalize(fs_in.WorldNormal);
	
	float light = calcLight(normal);
	
	vec3 col = texture(_Texture, fs_in.UV).rgb * calcLight(normal);
	FragColor = vec4(col,1.0);
}
