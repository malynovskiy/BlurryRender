#version 450 core
layout (location = 0) in vec3 aPos;

out vec2 aColor;

void main()
{
    gl_Position = vec4(aPos.xy, 0.0, 1.0); 
    aColor = (aPos.xy + vec2( 1.0 ) ) / vec2( 2.0 );      
}
