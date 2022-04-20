#version 460 core

out vec4 Colour;
uniform vec3 _Colour;

void main()
{
    Colour = vec4(_Colour, 1.0);
}