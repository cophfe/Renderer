//compute an environment map
//also holy shit this is insane, the people who developed this are geniuses (epic games, disney, some other people probably)
//based on https://learnopengl.com/PBR/IBL/Diffuse-irradiance
#version 460 core
#define PI_VALUE 3.141592653589793238462643
#define SAMPLE_COUNT 1024

in vec2 TexCoord;
out vec4 Colour;

uniform sampler2D _SkyMap;
uniform float _Roughness;

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
vec3 GetDirectionFromUV(vec2 uv)
{
	//i have no idea why i have to do this instead of uv -= 0.5 but it works soooooo
    uv.x += 0.25; 
    uv *= vec2(2 * PI_VALUE, PI_VALUE);
    float sLat =  sin(uv.y);
    return vec3(-sin(uv.x) * sLat, -cos(uv.y), cos(uv.x) * sLat);
}


//for more info on hammersley go to http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float RadicalInverseVdC(uint bits) 
{
    //this is used to compute random values from 0 - 1
    //inverse is because it involves reflecting a whole number about the decimal point, ie 1.0 -> 0.1 
    //radical is because it depends on the base, and radical is aparently a word for bases
    //it always stays between 0 and 1 because 9999999.0 flipped is 0.99999999
    //it is basically a way to generate evenly distributed 'random' values, which is useful for monte carlo methods, which is what we are doing 
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}
vec2 HammersleySequence(uint i)
{   
    //hammersley point set
    //used as a simple way to sample directions on a hemisphere (which is what we need to do obviously, using a hemisphere going around the normal)
    //is vec2 because spherical coordinates on unit sphere
    return vec2(float(i)/float(SAMPLE_COUNT), RadicalInverseVdC(i));
}  
vec3 ImportanceSampleGGX(vec2 coord, vec3 normal, float roughness)
{
	//this gives a sample that is biased towards an estimated halfway vector
    float r2 = roughness * roughness;
    float cTheta = sqrt((1.0 - coord.y) / (1.0 + (r2 * r2 - 1) * coord.y));
    float sTheta = sqrt(1.0 - cTheta*cTheta);
    
    // normal axis vectors
    vec3 up;
    if (abs(normal.z) < 0.9999)
		up = vec3(0,0,1);
	else
		up = vec3(1,0,0);
	vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);
	//convert to cartesian TS
    float phi = 2 * PI_VALUE * coord.x;
    vec3 cartesian = vec3(cos(phi) * sTheta, sin(phi) * sTheta, cTheta);
	return  tangent * cartesian.x + bitangent * cartesian.y + normal * cartesian.z;
}

void main()
{
    float tanFOV = tan(_FOV / 2.0);
    vec3 normal = GetDirectionFromUV(TexCoord * 0.5 + 0.5);
	float weight = 0;
	vec3 colour = vec3(0);

	//use monte carlo integration to get a estimation of the colour using random samples
	//'random' samples are biased to more important ones in order to converge on an answer faster
	for (int i = 0; i < SAMPLE_COUNT; ++i)
	{
		vec2 hemCoord = HammersleySequence(i);
		vec3 sampleHalfway = ImportanceSampleGGX(hemCoord, normal, _Roughness);
		vec3 lightDir = normalize(2*dot(sampleHalfway, normal) * sampleHalfway - normal);
		//ndl is used as the probability density of a sample 
		float nDL = max(dot(normal, lightDir), 0.0);
		colour += textureLod(_SkyMap, GetSphericalUV(lightDir), 0.0).xyz * nDL;
        weight += nDL;
	}

	//with the power of probability this should converge on a good answer
    Colour = vec4(colour/weight, 1.0);
    //Colour = vec4(vec3(1,0,0), 1.0);
}