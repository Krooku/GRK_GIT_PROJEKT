#version 430 core

uniform vec3 objectColor;
uniform vec3 lightDir;

uniform float offset;
in vec2 vertexTexNew;
in vec3 interpNormal;

void main()
{
	vec2 vertexTex = vertexTexNew;
	vec3 light = vec3(0.f, -1.f, 0.f);

	vec3 normal = normalize(interpNormal);

	float diffuse = max(dot(normal, light), 0.0);

	gl_FragColor = vec4(vec3(0.0, 0.0, 1.0) * diffuse, 0.2);
	

}
