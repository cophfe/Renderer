#version 460 core
layout (location = 4) in vec2 texCoord;

out vec2 TexCoord;
void main()
{ 
    gl_Position = vec4(texCoord,1.0, 1.0);
    TexCoord = texCoord; 
}