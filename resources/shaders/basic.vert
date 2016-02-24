#version 450

layout (std140, binding = 0) uniform frameUniforms 
{
    mat4 mvp;
} frameUniformsBuffer;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

layout (location = 0) out vec3 outNormal;

void main() 
{
   outNormal = normal;
   gl_Position = frameUniformsBuffer.mvp * vec4(position, 1.0f);

    // GL->VK conventions
   gl_Position.y = -gl_Position.y;
   gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
}