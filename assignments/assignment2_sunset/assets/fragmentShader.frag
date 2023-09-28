#version 450
out vec4 FragColor;
in vec2 UV;
uniform vec2 iRes;
uniform float iTime;
void main()
{
	vec2 uv = UV;

	float sine = abs(sin(iTime * 0.5));    
    float cose = abs(cos(iTime * 0.5));
    
    vec3 colorA = vec3(cose,0.0,0.2);
    float hill = mod((uv.x* 3.0), 0.4);
    if(hill > 0.2)
    {
        hill = 0.4;
    }
    else
    {
        hill = 0.0;
    }
       
    float hills = 1.0 - step(hill*1.0 + 0.0,uv.y + 0.0);
    vec2 circlePos = vec2(0.5, clamp(cose-0.4,-1.0, 0.7) - 0.1);
    vec2 circleUV = vec2(uv.x, uv.y * (iRes.y/iRes.x));
    float d = length(circleUV-circlePos)-0.10f;
    
    d = 1.0 - smoothstep(-0.05,0.2,d);
    vec3 color = mix(colorA, vec3(1.0, 1.0, 0.0),d);
    color = mix(color, vec3(0.2), hills);
    
    hill = mod((uv.x* 4.0), 0.3);
    if(hill < 0.2)
    {
        hill = 0.4;
    }
    else
    {
        hill = 0.0;
    }
       
    hills = 1.0 - step(hill*1.0 + 0.03,uv.y);
    color = mix(color, vec3(0.0),hills);
    
    FragColor = vec4(color,1.0);
}