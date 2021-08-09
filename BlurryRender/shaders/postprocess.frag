#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D maskTexture;

uniform vec2 resolution;

uniform int horizontal;
uniform int samples;
uniform float sigmaFactor;

const float pi = 3.1415926;

float sigma = float(samples) * sigmaFactor;
float s = 2 * sigma * sigma;
float s0 = pi * s;

float GaussianFilter(float r)
{
  return exp(-r * r / s) / s0;
}

vec3 GaussianBlur(sampler2D sp, vec2 uv, vec2 scale, int hor)
{
  vec3 pixel = vec3(0.0);
  float weightSum = 0.0;
  vec2 direction;

  if (hor == 1)
    direction = vec2(0, 1);
  else
    direction = vec2(1, 0);

  for (int i = -samples / 2; i < samples / 2; i++)
  {
    float weight = GaussianFilter(i);
    vec2 offset = direction * i;
    pixel += texture(screenTexture, uv + scale * offset).rgb * weight;
    weightSum += weight;
  }
  return pixel / weightSum;
}

void main()
{
  vec2 ps = 1.0 / textureSize(screenTexture, 0);
  vec3 bluredTexture = GaussianBlur(screenTexture, TexCoords, ps, horizontal);

  vec3 screenPixel = texture(screenTexture, TexCoords).rgb;
  float maskValue = texture(maskTexture, TexCoords).r;

  FragColor = vec4(mix(bluredTexture, screenPixel, maskValue), 1.0);
}
