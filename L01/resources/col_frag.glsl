#version 410 core

in vec3 fragColor;
out vec4 out_fragColor;

void main()
{
	vec3 normal = normalize(fragColor);
    out_fragColor = vec4(normal, 1.0);
}
