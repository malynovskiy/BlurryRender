#version 450 core

struct Material
{
  sampler2D diffuse;
  vec3 specular;
  float shininess;
};

struct Light
{
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
  vec3 textureDiffuse = vec3(texture(material.diffuse, TexCoords));
  vec3 ambient = textureDiffuse * light.ambient;

  // diffuse
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.position - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * textureDiffuse;

  // specular
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = (spec * material.specular) * light.specular;

  vec3 result = (ambient + diffuse + specular);
  FragColor = vec4(result, 1.0);
}
