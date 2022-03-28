#version 460 core
#define LIGHT_COUNT 8

struct LightData
{
    //vectors
    vec3 position;
    vec3 luminance; // contains colour 
    vec3 direction; //spot and directional light direction
    //point and spot light attenuation
    float constant;
    float linear;
    float quadratic;
    //spot light angle
    float minAngle; //starts to fall off
    float maxAngle; //finishes falling off
    //light type (0 is directional light, 1 is point light, 2 is spotlight)
    int type; 
};

struct MaterialData {
    float shininess; //between 0 and 300 (roughly, usually)
    float specularity; //between 0 and 1 (usually)
};

out vec4 Colour;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D _MainTex;
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
//although aie instructure variable names are so bad that it is practically impossible to understand without first comparing it to learnopenGL

vec3 GetPointLight(LightData light, vec3 normal, vec3 diffuseColour, vec3 surfaceToCamera)
{
    vec3 rayDirection = normalize(light.position - FragPos);  
    //attenuation
    float dist = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
    //diffuse light
    float diffuseValue =  max(dot(normal, rayDirection), 0.0);
    vec3 diffuse = diffuseColour * diffuseValue * light.luminance;
    //Specular Light
    vec3 reflection = reflect(-rayDirection, normal);
    float specularValue = _Material.specularity * pow(max(0.0f, dot(reflection, surfaceToCamera)), _Material.shininess);
    vec3 specular = specularValue * light.luminance;
    
    return attenuation * (diffuse + specular);
}

vec3 GetDirectionalLight(LightData light, vec3 normal, vec3 diffuseColour, vec3 surfaceToCamera)
{
    vec3 rayDirection = light.direction;  
    //diffuse light
    float diffuseValue =  max(dot(normal, rayDirection), 0);
    vec3 diffuse = diffuseColour * diffuseValue * light.luminance;
    //Specular Light
    vec3 reflection = reflect(-rayDirection, normal);
    float specularValue = _Material.specularity * pow(max(0.0f, dot(reflection, surfaceToCamera)), _Material.shininess);
    vec3 specular = specularValue * light.luminance;

    return diffuse;
}

vec3 GetSpotLight(LightData light, vec3 normal, vec3 diffuseColour, vec3 surfaceToCamera)
{
    vec3 rayDirection = normalize(light.position - FragPos);   //not the direction of the spotlight, the direction of the ray
    //attenuation
    float dist = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
    // spotlight falloff 
    float theta = dot(rayDirection, normalize(light.direction)); 
    float epsilon = light.minAngle - light.maxAngle;
    float falloff = clamp((theta - light.maxAngle) / epsilon, 0.0, 1.0);
    //diffuse light
    float diffuseValue =  max(dot(normal, rayDirection), 0.0);
    vec3 diffuse = diffuseColour * diffuseValue * light.luminance;
    //Specular Light
    vec3 reflection = reflect(-rayDirection, normal);
    float specularValue = _Material.specularity * pow(max(0.0f, dot(reflection, surfaceToCamera)), _Material.shininess);
    vec3 specular = specularValue * light.luminance;
    
    return falloff * attenuation * (diffuse + specular);
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 diffuseColour = texture(_MainTex, TexCoord).xyz;
    vec3 surfaceToCamera = normalize(_CameraPosition - FragPos);
    
    //Light Values
    vec3 ambient = _AmbientColour.xyz * diffuseColour;
    vec3 lightPower = vec3(0);
    lightPower += ambient;
    
    for(int i = 0; i < 1; i++)
    {
        switch(_Lights[i].type)
        {
            case 0: // directional light
            lightPower += GetDirectionalLight(_Lights[i], normal, diffuseColour, surfaceToCamera);
            break;

            case 1: // point light
            lightPower += GetPointLight(_Lights[i], normal, diffuseColour, surfaceToCamera);
            break;

            case 2: // spot light
            lightPower += GetSpotLight(_Lights[i], normal, diffuseColour, surfaceToCamera);
            break;
        }
    }

    Colour = vec4(lightPower, 1.0);
} 

