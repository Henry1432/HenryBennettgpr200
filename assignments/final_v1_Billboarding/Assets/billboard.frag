//again, very simpe, also draws as circle
#version 450
out vec4 FragColor;
in vec2 UV;
in vec3 Pos;

uniform vec3 _Color;
uniform bool _Circle;
uniform float _AlphaSet = 1;

void main(){
	if(_Circle)
	{
		vec2 center = vec2(Pos.x + 0.5, Pos.y + 0.5);
		vec2 offset = UV - center;
		float dist = sqrt(dot(offset, offset));
		if(dist < 0.4f)
		{
			FragColor = vec4(_Color, 1);
		}
		else
		{
			discard;
		}
	}
	else
	{
			FragColor = vec4(_Color,1.0);
	}
}