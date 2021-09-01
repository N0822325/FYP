#version 300 es

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec2 texCoord;

out vec4 outColour;
out vec2 vTexCoord;

uniform mat4 transform;

void main()
{
     gl_Position = transform * vec4(position, 1.0f);
     outColour = colour;
     vTexCoord = texCoord;
};
