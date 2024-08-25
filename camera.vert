#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTexCoord;

out vec3 ourColor;
out vec3 FragPos;
out vec3 ourNormal; 
out vec2 TexCoord;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(transform * vec4(aPos,1.0));

	ourNormal = mat3 (transpose(inverse(transform))) * aNormal;
	 
	gl_Position = projection * view * transform * vec4(aPos, 1.0f);

	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}