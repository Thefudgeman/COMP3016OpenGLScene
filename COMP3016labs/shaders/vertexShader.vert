#version 460

layout (location = 0) in vec3 posIn;
layout (location = 1) in vec3 normalIn;
layout (location = 2) in vec2 texCoordsIn;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
    FragPos = vec3(model * vec4(posIn, 1.0));
    Normal  = normalize(normalMatrix * normalIn);
    TexCoords = texCoordsIn;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
