#version 460 core

out vec4 Colour;
in vec2 TexCoord;
uniform sampler2D HDR;

vec3 Reinhard(vec3 colour)
{
    const float gamma = 1.2;
    
    vec3 mappedColour = colour / (colour + vec3(1.0));
    //gamma correct
    return pow(mappedColour, vec3(1.0 / gamma));
}
//reinhard
void main()
{       
    vec3 unmappedColour = texture(HDR, TexCoord).rgb;
    //Colour = vec4(Reinhard(unmappedColour), 1.0); //wow that looks ugly
    Colour = vec4(unmappedColour, 1.0);
}  