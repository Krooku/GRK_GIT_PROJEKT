#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelMatrix;
uniform float offset;

out vec3 interpNormal;

out vec2 vertexTexNew;

void main()
{
	vertexTexNew= vertexTexCoord;
	vertexTexNew.x += sin(vertexTexNew.y * 4*2*3.14159 + offset) / 100;
	
	vec3 vertexPositionNew = vertexPosition;
	vertexPositionNew.x += sin(vertexPositionNew.z * 4*2*3.14159 + offset) / 100;
	gl_Position = modelViewProjectionMatrix * vec4(vertexPositionNew, 1.0);
	interpNormal = (modelMatrix * vec4(vertexNormal, 0.0)).xyz;
}
