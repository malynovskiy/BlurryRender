#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform bool applyGradient;
uniform bool applyBlur;

const float offset = 1.0 / 300.0;  

const int kernelSize = 9;

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel[kernelSize] = float[](
      1.0 / 16, 2.0 / 16, 1.0 / 16,
      2.0 / 16, 4.0 / 16, 2.0 / 16,
      1.0 / 16, 2.0 / 16, 1.0 / 16  
    );
    
    vec3 result = vec3(0.0);
    vec3 sampleTex[kernelSize];
    
    if(applyBlur)
    {
      for(int i = 0; i < kernelSize; i++)
      {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
        result += sampleTex[i] * kernel[i];
      }

      if(applyGradient)
        FragColor = vec4(mix(vec3(texture(screenTexture, TexCoords)), result, TexCoords.s), 1.0);
      else
        FragColor = vec4(result, 1.0);
    }
    else
    {
      FragColor = vec4(vec3(texture(screenTexture, TexCoords)), 1.0);
    }
}

