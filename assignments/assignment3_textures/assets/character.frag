#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _Character;

void main()
{
	FragColor = texture(_Character,UV);
}
