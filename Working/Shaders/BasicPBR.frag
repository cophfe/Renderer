#version 460 core

#define LIGHT_COUNT 8
#define PI_VALUE 3.141592653589793238462643
#define E_VALUE 2.71828182845904523536028747135266249775724709369995

struct LightData
{
    //vectors
    vec3 position;
    vec3 luminance; // contains colour 
    vec3 direction; //spot and directional light direction
    //point and spot light attenuation
    float linear;
    float quadratic;
    //spot light angle
    float minAngle; //starts to fall off
    float maxAngle; //finishes falling off
    //light type (0 is directional light, 1 is point light, 2 is spotlight)
    int type; 
};

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
};

uniform MaterialData _Material;
//Samplers
uniform sampler2D _AlbedoMap;
uniform sampler2D _NormalMap;
uniform sampler2D _MetallicMap;
uniform sampler2D _RoughnessMap;
uniform sampler2D _AmbientOcclusionMap;
uniform samplerCube _CubeMap;
layout (std140, binding = 2) uniform Lighting
{
    LightData _Lights[LIGHT_COUNT];  //0
	vec4 _AmbientColour;			 //72 * LIGHT_COUNT
};
layout (std140, binding = 0) uniform CameraData
{
    mat4 _ViewProjectionMatrix;		
    mat4 _ProjectionMatrix;			
    mat4 _ViewMatrix;				
	mat4 _InverseViewMatrix;
    vec3 _CameraPosition;
    //mat3(transpose(inverse(model))) // Matrix for normals
};

//this is an amazing source for PBR:
//https://learnopengl.com/PBR/Theory
//it even explains a lot of the physical theory behind the calculations, and shows some of the approximations used in the industry
//this is suuuper useful for getting different approximations: https://www.jordanstevenstechart.com/physically-based-rendering

float Distribution (float nDH, float roughness)
{
    //Trowbridge-Reitz 
    float r2 = roughness*roughness;
    float div = nDH * nDH * (r2 - 1.0) + 1.0;
    return r2 / (div * div * PI_VALUE);

    //apparantely GGX is the most popular algorythm
    //float nDH2 = nDH * nDH;
    //float v = roughness / (nDH2 * (r2 + (1- nDH2)/nDH2));
    //return v * v / PI_VALUE;
    
    //aie instructure uses beckman distribution instead
    //I assume this is because it can use the same roughness value as oren nayer?
    //float nDH2 = nDH * nDH;
    //float ex = (nDH2 - 1) / (nDH2 * r2);
    //return pow( E_VALUE, ex) / (r2 * nDH2 * nDH2);
}

float Fresnel(vec3 normal, vec3 viewDir)
{
    //pass in vec3 normal, vec3 halfDir, float metallicity, vec3 surfaceColour
    //vec3 f0 = mix(vec3(0.05), surfaceColour, metallicity);
    //float cosTheta = max(dot(normal, halfDir), 0);
    //return f0 + (1 - f0) * pow(1 - cosTheta, 5);

    //pass in vec3 normal, vec3 viewDir for this one
    float nDV = 1.0 - dot(normal, viewDir);
    return 0.05 + (1 - 0.05) * nDV * nDV * nDV * nDV * nDV;
}

float Geometry(float nDV, float nDL, float roughness)
{
    //here is shlikk beckmann + ggx function used in smith 
    //remapped roughness
    float k = roughness + 1;
    k = k * k / 8.0;
    //k with image based lighting:
    //float k = roughness * roughness / 2
    return (nDV / (nDV * (1.0 - k) + k)) * (nDL / (nDL * (1.0 - k) + k));

    //this is the function AIE uses, they don't say what one it is
    //research says its the cook torrance gsf, which i guess makes sense.
    //float x = 2.0f * nDH / nDH;
    //return min(1.0, min(x * nDV, x * nDL));
}

vec3 CookTorranceBRDF(vec3 lightDir, vec3 viewDir, vec3 surfaceNorm, float roughness, float metallicity, vec3 surfaceColour)
{
    vec3 halfDir = normalize(lightDir + viewDir);    
    //now the specular part
    float nDH = max(dot(surfaceNorm, halfDir), 0.0);
    float nDV = max(dot(viewDir, surfaceNorm), 0.0);
    float nDL = max(dot(lightDir, surfaceNorm), 0.0);

    float distribution = Distribution(nDH, roughness);
    //vec3 fresnel = Fresnel(surfaceNorm, halfDir, metallicity, surfaceColour);
    float fresnel = Fresnel(surfaceNorm, viewDir);
    //Smith's Schlick-GGX
    float geometry = Geometry(nDV, nDL, roughness);
    float specular = (distribution*fresnel*geometry)/(4 * nDV * nDL);

    //the diffuse part of the BRDF
    //i think aie uses oren-nayar diffuse reflectance as a replacement for this?
    //kD is the proportion of total energy being turned into diffuse lighting, which is just 1.0 - the fresnel term
    vec3 diffuse = (1.0f - fresnel) * surfaceColour/PI_VALUE;

    return vec3(distribution);
}

//For point and spotlights
float CalculateAttenuation(LightData light)
{
    float dist = length(light.position - vertexOutput.fragPos);
    return  1.0 / (1.0 + light.linear * dist + light.quadratic * dist * dist);
}

//for spotlights
float CalculateFalloff(LightData light, vec3 rayDirection)
{
    float theta = dot(rayDirection, normalize(light.direction)); 
    float epsilon = light.minAngle - light.maxAngle;
    return clamp((theta - light.maxAngle) / epsilon, 0.0, 1.0);
}

void main()
{
    //i will probably need to change everything so it's all in tangent space instead of world space 

    //Get normal
	vec3 normalTS = texture(_NormalMap, vertexOutput.texCoord).xyz * 2 - 1;
	vec3 normalWS = normalize(vertexOutput.matrixTBN * normalTS); //TBN Matrix is in world space

    vec3 viewDirection = normalize(vertexOutput.fragPos - _CameraPosition);

    //get fragment properties
    float metallicity = texture(_MetallicMap, vertexOutput.texCoord).r;
    float roughness = clamp(_Material.roughness * texture(_RoughnessMap, vertexOutput.texCoord).r, 0, 1);
    vec3 surfaceColour = texture(_AlbedoMap, vertexOutput.texCoord).xyz;
    
    vec3 lightDirection = normalize(vertexOutput.fragPos - _Lights[0].position);
    
    vec3 colour = CookTorranceBRDF(lightDirection, viewDirection, normalTS, roughness, metallicity, surfaceColour); //* CalculateAttenuation(_Lights[0]);
    Colour = vec4(colour, 1);
}