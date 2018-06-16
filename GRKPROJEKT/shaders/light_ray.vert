#version 430 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;

uniform float offset;

out vec3 interpNormal;
out vec2 vertexTexNew;
void main()
{
	vertexTexNew = vertexPosition;
	vertexTexNew.x += sin(40.0 * vertexTexNew.y * 4*2*3.14159 + offset) / 100;
	

	vec2 fragCoord = vertexPosition;
	
	
	gl_Position = vec4(fragCoord.x, fragCoord.y, 0.0, 1.0);
	interpNormal = (vec3(fragCoord.x + 0.5, fragCoord.y - 1.2, 0.0)).xyz;
	

}
