#version 460 core

out vec4 Colour;
in vec2 TexCoord;
uniform sampler2D HDR;

const float exposure = 1.0;
const float gamma = 2.2;

vec3 Reinhard(vec3 colour)
{
    vec3 mappedColour =  vec3(1.0) - exp(-colour * exposure);
    //gamma correct
    return pow(mappedColour, vec3(1.0 / gamma));
}
//reinhard
void main()
{       
    vec3 unmappedColour = texture(HDR, TexCoord).rgb;
    //Colour = vec4(Reinhard(unmappedColour), 1.0); 
    //Colour = vec4((unmappedColour - vec3(1)) * 1000, 1.0); //check if there is any colour with value > 1 (there is)
    Colour = vec4(unmappedColour, 1.0);
}  