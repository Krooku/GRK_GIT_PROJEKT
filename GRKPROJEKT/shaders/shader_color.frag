#version 430 core

uniform vec3 objectColor;
uniform vec3 lightDir;

uniform float time;

in vec3 interpNormal;

void main()
{
	vec3 normal = normalize(interpNormal);

	float diffuse = max(dot(normal, -lightDir), 0.0);
	gl_FragColor = vec4(objectColor * diffuse, 1.0);
}
