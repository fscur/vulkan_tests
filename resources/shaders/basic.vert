#version 450

layout (std140, binding = 0) uniform frameUniforms 
{
    mat4 vp;
} frameUniformsBuffer;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

layout (location = 0) out vec2 outTexCoord;
layout (location = 1) out vec3 outNormal;

void main() 
{
   gl_Position = frameUniformsBuffer.vp  *  vec4(position, 1.0f);

    // GL->VK conventions
   gl_Position.y = -gl_Position.y;
   gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;

   outNormal = normal;
   outTexCoord = texCoord;
}