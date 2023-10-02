#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _BrickTexture;
uniform sampler2D _NoiseTexture;

void main(){
	float push = texture(_NoiseTexture, UV).r;
	vec2 brickUV = (UV + (push*0.1) -0.05);
	FragColor = texture(_BrickTexture,brickUV);
}
