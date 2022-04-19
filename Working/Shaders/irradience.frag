#version 460 core
#define PI_VALUE 3.141592653589793238462643

in vec2 TexCoord;
out vec4 Colour;
uniform sampler2D _SkyMap;
layout (std140, binding = 0) uniform CameraData
{
    mat4 _ViewProjectionMatrix;		
    mat4 _ProjectionMatrix;			
    mat4 _ViewMatrix;				
	mat4 _InverseViewMatrix;
    vec3 _CameraPosition;
    float _Aspect;
    float _FOV;
};

vec2 GetSphericalUV(vec3 rayDir)
{
    vec2 uv = vec2(atan(rayDir.z, rayDir.x), asin(rayDir.y)) * vec2(0.1591, 0.3183);
    uv += 0.5; 
    return uv;
}

//uv needs to be between 0 and 1 for this to work
vec3 GetDirectionFromUV(vec2 uv)
{
    //convert uv to be in spherical coordinates
    uv -= 0.5;
    uv *= vec2(2 * PI_VALUE, PI_VALUE);
    //now uv should be spherical coordinates now can be mapped to cartesian coordinates 
    float sLat = sin(uv.y);
    return vec3(sin(uv.x) * sLat, cos(uv.y), cos(uv.x) * sLat);
   // return vec3(cos(uv.x) * sLat, sin(uv.x) * sLat, sin(uv.z))
}

//I am extrememly running out of time so this is mostly just learnopengl.com/PBR/IBL/Diffuse-irradiance
//biggest difference is GetDirectionFromUV which is custom so that I can use equirectangular maps because again I'm running out of time
void main()
{
    float tanFOV = tan(_FOV / 2.0);
    //i'm not sure if this is right but I do know it's identical to the skybox raydirection. so at worst they're both wrong
    vec3 normal = GetDirectionFromUV(TexCoord * 0.5 + 0.5);
    vec3 irradiance = vec3(0.0); 
    vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), normal));
    vec3 up = normalize(cross(normal, right));
    float sDelta = 0.025;
    float sCount = 0.0; 

    //theta and phi are spherical coordinates
    //using them to sample the irradience coming from different parts of the sky map
    //since we are sampling a hemisphere of radiances phi is between 0 and 2pi (0-360 degrees) and theta is between 0 and pi/2 (0-90 degrees)
    for(float phi = 0.0; phi < 2.0 * PI_VALUE; phi += sDelta)
    {
        for(float theta = 0.0; theta < PI_VALUE/2.0; theta += sDelta)
        {
            // spherical to cartesian in TS
        	//When I was researching I found that this transformation from spherical to cartesian coordinates, which is from learnopengl, uses the wrong axis??
			//it uses the maths axis, where up is z, right is y, and backward is x. same as mathworld.wolfram.com/SphericalCoordinates.html and https://keisan.casio.com/exec/system/1359534351
			//but if I correct for it, it returns very obviously incorrect results. so i guess it is right somehow even if the axis are wrong
            float sLat = sin(theta); 
            vec3 tangentSample = vec3(sLat * cos(phi), sLat * sin(phi), cos(theta));
            // TS to WS
        	vec3 worldSample = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 
            irradiance += texture(_SkyMap, GetSphericalUV(worldSample)).rgb * cos(theta) * sin(theta);
            sCount++;
        }
    }
    irradiance = PI_VALUE * irradiance * (1.0 / float(sCount));
    Colour = vec4(irradiance, 1.0);
}