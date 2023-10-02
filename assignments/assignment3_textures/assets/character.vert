#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
out vec2 UV;

uniform float iTime;

void main(){
	UV = vUV;

	vec3 newPos = vPos * 0.5;
	newPos.x += sin(iTime*2) * 0.5;
	newPos.y += cos(iTime*2) * 0.5;

	gl_Position = vec4(newPos,1.0);
}