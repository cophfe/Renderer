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

//spherical/equirectangular projection sampling
vec2 GetSphericalUV(vec3 rayDir)
{
	//ray dir is actually cartesian coordinates on a unit sphere (so r = 1)
	//the magic number used here is 1/2Pi and 1/pi, which converts the spherical coordinates to have a range of 1 (initially longitude has range of 2pi and lat has range of 1pi)
	//teeeechnically its not longitude and latitude, its azimuthal angle and polar angle, but I can't remember which one of those is x and which is y so longitude and latitude works better
	vec2 uv = vec2(atan(rayDir.z, rayDir.x), asin(rayDir.y)) * vec2(0.1591, 0.3183);
	//make between 0 and 1, not -0.5 and 0.5
	uv += 0.5;
	return uv;
}

vec3 GetCameraRayFromScreenPos(float tanFOV, vec2 screenPos)
{
	//screen pos between -1,-1 and 1,1
	vec3 rayDirection = vec3(-_Aspect * tanFOV * screenPos.x, -tanFOV * screenPos.y, 1.0);
	return vec3(_InverseViewMatrix * vec4(normalize(rayDirection), 0.0));
}

void main()
{
	//this sky'box' is actually a screen sized quad, that uses camera information to render an equirectangular projection of an image
	float tanFOV = tan(_FOV / 2.0);
	vec3 rayDirection = GetCameraRayFromScreenPos(tanFOV, TexCoord);
	vec2 uv = GetSphericalUV(rayDirection);
	//need to sample the 0 lod else there is a seam caused by atan2 where mip level is maxed out
	Colour = textureLod(_SkyMap, uv, 0.0); 
	//Colour = texture(_SkyMap, uv); 
}