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


uniform bool ForestTree;

out vec4 FragColor;

uniform Light light;
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_Roughness;
uniform sampler2D texture_Metallic;

void main()
{
    vec3 texColor =  texture(texture_diffuse1, TexCoords).rgb;
    float roughness = texture(texture_Roughness, TexCoords).r;
    float metallic = texture(texture_Metallic, TexCoords).r;

    if(texture(texture_diffuse1, TexCoords).a < 0.5)
    {
        discard;
    }
    // Ambient
    vec3 ambient = light.ambient * texColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texColor* metallic;

    // Specular

    vec3 specular = new vec3(0.0,0.0,0.0);

    if(!ForestTree) //makes sure ForestTree model has no specular lighting otherwise some leaves appear very bright
    {
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float specPower = mix(128.0, 4.0, roughness);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), specPower);
        specular = light.specular * spec* metallic;
    }

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
