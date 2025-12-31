#version 460
//Colour value to send to next stage
out vec4 FragColor;


struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
//Colour coordinates from last stage
in vec3 colourFrag;

uniform Light light;
uniform vec3 viewPos;

void main()
{
     // Ambient
    vec3 ambient = light.ambient * colourFrag;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * colourFrag;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = light.specular * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}