//simple shader, just wanted it to not interact with lights so it feels more like u1
#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out vec2 UV;
out vec3 Pos;
uniform mat4 _Model;
uniform mat4 _ViewProjection;

out vec4 pixalPos;
void main(){
	UV = vUV;
	Pos = vPos;

	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}