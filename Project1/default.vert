#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNormal;
layout (location = 4) in vec3 aTangent;

out vec3 color;
out vec2 texCoord;

out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;

uniform float scale;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0);

    color = aColor;
    texCoord = aTex;

    // Transformar la normal y la tangente al espacio del mundo
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    Tangent = normalize(mat3(model) * aTangent);

    // La bitangente se obtiene mediante producto cruz
    Bitangent = normalize(cross(Normal, Tangent));
}