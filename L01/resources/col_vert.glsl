#version 410 core

uniform mat4 P;
uniform mat4 MV;

layout(location = 0) in vec4 aPos;

out vec3 fragColor;

void main()
{
	gl_Position = P * MV * aPos;
	fragColor = aPos.xyz;
}
