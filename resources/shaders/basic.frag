#version 450

layout (location = 0) out vec4 outColor;

layout (location = 0) in vec2 inTexCoord;
layout (location = 1) in vec3 inNormal;

void main()
{
    vec3 directionalLight = normalize(vec3(5.0f, 2.0f, 1.0f));
    float lightIncidence = max(dot(directionalLight, inNormal), 0.0f);
    vec3 color = lightIncidence * vec3(0.6f, 0.2f, 0.5f);

    outColor = vec4(color, 1.0f);
}