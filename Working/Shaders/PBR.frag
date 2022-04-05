#version 460 core
#define LIGHT_COUNT 8

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

struct PBRMaterial {
    float shininess; //between 0 and 300 (roughly, usually)
    float specularity; //between 0 and 1 (usually)
};

struct V2F {
    mat3 matrixTBN;
    vec3 normal;
    vec3 fragPos;
    vec2 texCoord;
};

out vec4 Colour;

in V2F vertexOutput;

uniform sampler2D _DiffuseMap;
uniform sampler2D _NormalMap;
uniform sampler2D _SpecularMap;
uniform MaterialData _Material;

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

//Lighting based on learnopenGL and aie instructure
//although aie instructure variable names are so bad that it is practically impossible for me to understand without using knowledge from learnopengl

vec3 CalculateDiffuse(LightData light, vec3 normal, vec3 rayDirection, vec3 diffuseColour)
{
    float diffuseValue =  max(dot(normal, rayDirection), 0.0);
    return diffuseColour * diffuseValue * light.luminance;
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

vec3 CalculateSpecular(LightData light, vec3 normal, vec3 rayDirection)
{
    vec3 reflection = reflect(-rayDirection, normal);
    float specularValue = _Material.specularity * pow(max(0.0f, dot(reflection, surfaceToCamera)), _Material.shininess);
    return specularValue * light.luminance;
}

vec3 GetDirectionalLight(LightData light, vec3 normal, vec3 diffuseColour, vec3 surfaceToCamera)
{
    vec3 rayDirection = light.direction;  
    //diffuse light
    vec3 diffuse = CalculateDiffuse(light, normal, rayDirection, diffuseColour);
    //Specular Light
    vec3 specular = CalculateSpecular(light, normal, rayDirection);

    return diffuse + specular;
}

vec3 GetPointLight(LightData light, vec3 normal, vec3 diffuseColour, vec3 surfaceToCamera)
{
    vec3 rayDirection = normalize(light.position - vertexOutput.fragPos);  
    //attenuation
    float attenuation = CalculateAttenuation(light);
    //diffuse light
    vec3 diffuse = CalculateDiffuse(light, normal, rayDirection, diffuseColour);
    //Specular Light
    vec3 specular = CalculateSpecular(light, normal, rayDirection);
    
    return attenuation * (diffuse + specular);
}

vec3 GetSpotLight(LightData light, vec3 normal, vec3 diffuseColour, vec3 surfaceToCamera)
{
    //not the direction of the spotlight, the direction of the ray (light eminates from light position in all directions)
    vec3 rayDirection = normalize(light.position - vertexOutput.fragPos); 
    
    //attenuation
    float attenuation = CalculateAttenuation(light);
    // spotlight falloff 
    float falloff = CalculateFalloff(light, rayDirection);
    //diffuse light
    float diffuseValue = CalculateDiffuse(light, normal, rayDirection, diffuseColour);
    //Specular Light
    vec3 specular = CalculateSpecular(light, normal, rayDirection);
    
    return falloff * attenuation * (diffuse + specular);
}

void main()
{
	//Get normal
	vec3 normalTS = texture(_NormalMap, vertexOutput.texCoord).xyz * 2 - 1;
	vec3 normalWS = normalize(vertexOutput.matrixTBN * normalTS); //TBN Matrix is in world space

    vec3 diffuseColour = texture(_DiffuseMap, vertexOutput.texCoord.xy).xyz;
    vec3 surfaceToCamera = normalize(_CameraPosition - vertexOutput.fragPos);
    
    //Light Values
    vec3 ambient = _AmbientColour.xyz * diffuseColour;
    vec3 lightPower = vec3(0);
    lightPower += ambient;
    
    for(int i = 0; i < LIGHT_COUNT; i++) //CHANGE TO LIGHT_COUNT
    {
        switch(_Lights[i].type)
        {
            case 0: // directional light
            lightPower += GetDirectionalLight(_Lights[i], normalWS, diffuseColour, surfaceToCamera);
            break;

            case 1: // point light
            lightPower += GetPointLight(_Lights[i], normalWS, diffuseColour, surfaceToCamera);
            break;

            case 2: // spot light
            lightPower += GetSpotLight(_Lights[i], normalWS, diffuseColour, surfaceToCamera);
            break;
        }
    }

    Colour = vec4(lightPower, 1.0);
} 

