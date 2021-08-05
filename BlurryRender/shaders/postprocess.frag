
#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int horizontal;
uniform int samples;
uniform float sigmaFactor;

const float pi = 3.1415926;

float sigma = float(samples) * sigmaFactor;
float s = 2 * sigma * sigma; 
float s0 = pi * s; 

float gauss(float r)
{
  return exp(-r * r / s) / s0;
}

vec3 gaussianBlur(sampler2D sp, vec2 uv, vec2 scale, int hor)
{
  vec3 pixel = vec3(0.0);
  float weightSum = 0.0;
  float weight;
  vec2 offset;

  if(hor == 1)
  {
    for(int j = -samples / 2; j < samples / 2; j++)
    {
        offset = vec2(0, j);
        weight = gauss(j);
        pixel += texture(screenTexture, uv + scale * offset).rgb * weight;
        weightSum += weight;
    }
  }
  if(hor == 0)
  {
    for(int i = -samples / 2; i < samples / 2; i++)
      {
          offset = vec2(i, 0);
          weight = gauss(i);
          pixel += texture(screenTexture, uv + scale * offset).rgb * weight;
          weightSum += weight;
      }
  }
  return pixel / weightSum;
}

void main()
{
  vec2 ps = vec2(1.0) / vec2(1920.0,1080.0);
  if(TexCoords.x > 0.5)
  {
    vec3 bluredTexture = gaussianBlur(screenTexture, TexCoords, ps, horizontal);
    FragColor = vec4(bluredTexture.rgb, 1.0);
  }  
  else
  {
    FragColor = vec4(texture(screenTexture, TexCoords).rgb, 1.0);
  }
}

