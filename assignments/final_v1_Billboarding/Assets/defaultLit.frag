#version 450
out vec4 FragColor;

in Surface{
	vec2 UV; //Per-fragment interpolated UV
	vec3 WorldPosition; //Per-fragment interpolated world position
	vec3 WorldNormal; //Per-fragment interpolated world normal
}fs_in;

//light set up
struct Light
{
	vec3 position;
	vec3 color;
};
#define MAX_LIGHTS 4
uniform Light _Lights[MAX_LIGHTS];
struct LightSettings
{
	float _AmbientK;
	float _DiffuseK;
	float _SpecularK;
	float _Shiny;
};
LightSettings _LightSettings;

uniform sampler2D _Texture;

uniform vec3 _CamPos;

vec3 calcLight(vec3 normal){
	vec3 totalSetLight;
	for(int i = 0; i < MAX_LIGHTS; i++)
	{
		vec3 lightDir = normalize(_Lights[i].position - fs_in.WorldPosition);
		vec3 halfVec = normalize(lightDir + normalize(_CamPos - fs_in.WorldPosition));
		float defuse = _LightSettings._DiffuseK * max(dot(lightDir,normal),0.0);
		float specular = _LightSettings._SpecularK * pow(max(dot(halfVec,normal),0.0), _LightSettings._Shiny);
		totalSetLight += _Lights[i].color * (defuse + specular + _LightSettings._AmbientK);
	}

	return (totalSetLight);
}

void main(){
	_LightSettings._AmbientK = 0.1f;
	_LightSettings._DiffuseK = 0.4;
	_LightSettings._SpecularK = 0.5;
	_LightSettings._Shiny = 32.0;

	vec3 normal = normalize(fs_in.WorldNormal);
	
	//float light = calcLight(normal);
	vec3 TotalCol;
	vec3 col = texture(_Texture, fs_in.UV).rgb * calcLight(normal);

	FragColor = vec4(col,1.0);
}
