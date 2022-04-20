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

//https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/ 
vec3 ACES(vec3 colour)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((colour*(a*colour+b))/(colour*(c*colour+d)+e), 0, 1);
}
//reinhard
void main()
{       
    vec3 unmappedColour = texture(HDR, TexCoord).rgb;
    //Colour = vec4(unmappedColour, 1.0);
    Colour = vec4(ACES(unmappedColour), 1.0); 
    //Colour = vec4((unmappedColour - vec3(1)) * 1000, 1.0); //check if there is any colour with value > 1 (there is)
}  