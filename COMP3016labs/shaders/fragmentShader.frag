#version 460 core

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform Light light;
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;

void main()
{
    vec3 texColor = texture(texture_diffuse1, TexCoords).rgb;

    // Ambient
    vec3 ambient = light.ambient * texColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = light.specular * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
