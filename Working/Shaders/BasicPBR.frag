#version 460 core

#define PI_VALUE 3.141592653589793238462643

struct V2F {
    mat3 matrixTBN;
    vec3 normal;
    vec3 fragPos;
    vec2 texCoord;
};

out vec4 Colour;
in V2F vertexOutput;

struct MaterialData
{
    float roughness;
    float  
}
uniform MaterialData _Material;
//Samplers
uniform sampler2D _AlbedoMap;
uniform sampler2D _NormalMap;
uniform sampler2D _MetallicMap;
uniform sampler2D _RoughnessMap;
uniform sampler2D _AmbientOcclusionMap;
uniform samplerCube _CubeMap;

//temporary
vec3 lightPosition = vec3(0);
vec3 lightColor = vec3(1.0, 0.8, 0.9);

vec3 cameraPosition = vec3(0);

//this is an amazing source for PBR:
//https://learnopengl.com/PBR/Theory
//it even explains a lot of the physical theory behind the calculations, and shows some of the approximations used in the industry

float Distribution (vec3 normal, vec3 halfwayDirection, float roughness)
{
    //Trowbridge-Reitz GGX
    float r2 = roughness*roughness;
    float nDH = max(dot(normal, halfwayDirection), 0.0);
    float div = nDH * nDH * (r2 - 1.0) + 1.0);
    return r2 / (div * div * PI_VALUE);

    //aie instructure uses beckman distribution instead
    //float nDH2 = nDH * nDH;
    //float ex = (nDH2 - 1) / (nDH2 * r2);
    //return pow( E_VALUE, ex) / (r2 * nDH2 * nDH2);
}

float Fresnel()
{
    //Fresnel-Schlick 

}
vec3 CookTorranceBRDF(vec3 lightDir, vec3 viewDir, vec3 surfaceNorm, float roughness)
{
    //the diffuse part of the BRDF
    vec3 diffuse = kD * AlbedoColour/PI_VALUE;
    
    float distribution = 
    float fresnel = 
    //Smith's Schlick-GGX
    float geometry = 
    vec3 specular = kS * (distribution*fresnel*geometry)/(4 * dot(viewDir * surfaceNorm) * dot(lightDir * surfaceNorm));
}

void main()
{
    //Get normal
	vec3 normalTS = texture(_NormalMap, vertexOutput.texCoord).xyz * 2 - 1;
	vec3 normalWS = normalize(vertexOutput.matrixTBN * normalTS); //TBN Matrix is in world space

    //get directions needed
    vec3 lightDirection = normalize(lightPosition - vertexOutput.fragPos);
    vec3 viewDirection = normalize(cameraPosition - vertexOutput.fragPos);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);

    float kS = akjdnawlkdn;
    float kD = 1.0 - kS;
}