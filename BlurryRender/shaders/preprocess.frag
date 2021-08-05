#version 450 core

in vec2 aColor;
out vec4 FragColor;

void main()
{
    const int factor = 80;
    vec2 size = vec2(1920/factor, 1080/factor);

    float total = floor(aColor.x*size.x) +
                  floor(aColor.y*size.y);

    bool isEven = mod(total,2.0)==0.0;
    vec4 col1 = vec4(1.0,0.0,0.0,1.0);
    vec4 col2 = vec4(1.0,1.0,1.0,1.0);
    FragColor = (isEven)? col1:col2;
}
