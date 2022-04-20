#version 460 core

#define LIGHT_COUNT 8
#define ROUGHNESS_MIP_MAX 5.0
#define PI_VALUE 3.141592653589793238462643
#define E_VALUE 2.71828182845904523536028747135266249775724709369995

struct LightData
{
    //light pos
    vec3 position;
    //starts to fall off
    float minAngle; 
    // contains colour and brightness
    vec3 luminance; 
    //finishes falling off
    float maxAngle;
    //spot and directional light direction
    vec3 direction; 
    //light type (0 is directional light, 1 is point light, 2 is spotlight)
    int type; 
    float radius; //light radius used for falloff
    //note: 12 unused bytes here if needed
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
    float metallicity;
};

uniform MaterialData _Material;
//Samplers
uniform sampler2D _AlbedoMap;
uniform sampler2D _NormalMap;
uniform sampler2D _MetallicMap;
uniform sampler2D _RoughnessMap;
uniform sampler2D _AmbientOcclusionMap;
uniform sampler2D _IrradienceMap;
uniform sampler2D _PrefilterMap;
uniform sampler2D _BRDFMap; 

layout (std140, binding = 2) uniform Lighting
{
    LightData _Lights[LIGHT_COUNT];  //0
	vec3 _AmbientColour;			 //72 * LIGHT_COUNT
    
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

float GetLuminance(vec3 colour)
{
    return 0.2126*colour.r + 0.7152*colour.g + 0.0722*colour.b;
}

vec2 GetSphericalUV(vec3 rayDir)
{
    vec2 uv = vec2(atan(rayDir.z, rayDir.x), asin(rayDir.y)) * vec2(0.1591, 0.3183);
    uv += 0.5; 
    return uv;
}

//most used sources for PBR and Image based lighting:
//https://learnopengl.com/PBR/Theory
//https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_slides.pdf
//https://www.jordanstevenstechart.com/physically-based-rendering

float Distribution (float nDH, float roughness)
{
    float r2 = roughness*roughness;
    float r4 = r2*r2;

	//Trowbridge-Reitz 
    //float div = nDH * nDH * (r2 - 1.0) + 1.0;
    //return r2 / (div * div * PI_VALUE);

    //apparantely GGX is the most popular algorithm
    float v = (nDH * nDH * (r4 - 1.0) + 1.0);
    return r4 / (PI_VALUE * v * v);
    
    //aie instructure uses beckman distribution instead
    //I assume this is because it can use the same roughness value as oren nayer?
    //float nDH2 = nDH * nDH;
    //float ex = (nDH2 - 1) / (nDH2 * r2);
    //return pow( E_VALUE, ex) / (r2 * nDH2 * nDH2);
}

vec3 Fresnel(vec3 viewDir, vec3 halfDir, vec3 r0)
{
    //r0 is basically the base surface reflectance
    //I'm using this model because it allows 
    // for transitioning between metallic and non metallic easily, by lerping r0 to the colour of the metal if it is a metallic object 

    //pass in vec3 normal, vec3 halfDir (return vec3)
    float cosTheta = max(dot(viewDir, halfDir), 0);
    return r0 + (1.0 - r0) * pow(clamp(1 - cosTheta, 0, 1), 5.0);
    
    //pass in vec3 normal, vec3 viewDir, float f0 (return float)
    // float nDV = 1.0 - dot(normal, viewDir);
    // return 0.05 + (1 - 0.05) * nDV * nDV * nDV * nDV * nDV;
}

float Geometry(float nDV, float nDL, float roughness)
{
    //here is shlikk beckmann + ggx function used in smith 
    //remapped roughness
    float k = roughness + 1;
    k = (k * k) / 8.0;
    //k with image based lighting:
    //float k = roughness * roughness / 2;
    return (nDV / (nDV * (1.0 - k) + k)) * (nDL / (nDL * (1.0 - k) + k));

    //this is the function AIE uses, they don't say what one it is
    //research says its the cook torrance gsf, which i guess makes sense, seeing as this is cook torrance.
    //float x = 2.0f * nDH / nDH;
    //return min(1.0, min(x * nDV, x * nDL));
}

//For point and spotlights
float CalculateAttenuation(LightData light)
{
    float dist = length(light.position - vertexOutput.fragPos);
    //return  (light.radius * light.radius) / (dist * dist);

    //using the model from UE4 which has some cool features like not blowing up up close and a maximum radius of influence
    float falloff = clamp(pow(1 - pow(dist/light.radius, 4), 2), 0, 1) / (pow(dist, 2) + 1);
    return falloff;
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

    //get all fragment values
    float ambientOcclusion = texture(_AmbientOcclusionMap, vertexOutput.texCoord).r;
    float metallicity = clamp(_Material.metallicity * texture(_MetallicMap, vertexOutput.texCoord).r, 0, 1);
    float roughness = clamp(_Material.roughness * texture(_RoughnessMap, vertexOutput.texCoord).r, 0, 1);
    vec3 albedoColour = texture(_AlbedoMap, vertexOutput.texCoord).xyz; 
	vec3 normalTS = texture(_NormalMap, vertexOutput.texCoord).xyz * 2 - 1;
	vec3 normalWS = normalize(vertexOutput.matrixTBN * normalTS); //TBN Matrix is in world space
    vec3 irradiance = textureLod(_IrradienceMap, GetSphericalUV(-normalWS), 0.0).rgb;

    vec3 viewDirection = normalize(_CameraPosition - vertexOutput.fragPos);
    float nDV = max(dot(normalWS, viewDirection), 0.0);
    vec3 r0 = mix(vec3(0.05), albedoColour, metallicity);

	vec3 outputColour = vec3(0);
    for (int i = 0; i < LIGHT_COUNT; i++)
    {
        //note: should check type of light to get these 2 values
		vec3 lightDirection;
		float lightPowerMultiplier = 1.0;

		//i am using this like a c switch when I should be using it as a glsl switch
		//aka i am concerned this breaks the non branchingness of gpus. hopefully we good? 
		switch(_Lights[i].type)
        {
            case 0: // directional light
			lightDirection = -_Lights[i].direction;
            break;

            case 1: // point light
        	lightDirection = normalize(_Lights[i].position - vertexOutput.fragPos);
			lightPowerMultiplier = CalculateAttenuation(_Lights[i]);
            break;

			case 2: // spot light
        	lightDirection = normalize(_Lights[i].position - vertexOutput.fragPos);
			lightPowerMultiplier = CalculateFalloff(_Lights[i], lightDirection) * CalculateAttenuation(_Lights[i]);
			break;
        }
        vec3 lightPower = _Lights[i].luminance * lightPowerMultiplier;

    	vec3 halfDir = normalize(lightDirection + viewDirection);
        float nDH = max(dot(normalWS, halfDir), 0.0);
        float nDL = max(dot(normalWS, lightDirection), 0.0);

    	float distribution = Distribution(nDH, roughness);
        //this fresnel literally doesn't take into account normal so it doesn't make sense that it works
        //vec3 fresnel = Fresnel(viewDirection, halfDir, r0);  and it doesn't work, for me, but it does for learnopengl somehow so thats interesting
        vec3 fresnel = Fresnel(viewDirection, halfDir, r0);
    	float geometry = Geometry(nDV, nDL, roughness);
    	
        //float oneMinusCosTheta = 1 - max(dot(viewDir, halfDir), 0);
        // return r0 + (1.0 - r0) * pow(oneMinusCosTheta, 5.0);

		vec3 s = (distribution*fresnel*geometry) 
			/ max(4 * nDV * nDL, 0.00001); //the 0.00001 is super important, it will break sometimes without it
    
		//fresnel basically works as the proportion of energy being reflected, kD will be the inverse of that (whatever energy was not reflected should be diffused) 
		//except since metals don't have a diffuse term (aka the diffuse energy is 100% absorbed by the object), diffuse energy has to be multiplied by inverse metalness to basically lerp it to zero based on metalness
		//now we don't need a seperate pipeline for metal and non metal stuff, and they can be lerped between for smooth edges n stuff (aka physics got absolutely scammed, hooray)
		vec3 kD = (vec3(1) - fresnel) * (1 - metallicity);

		//final output luminance (timesed by nDL because i have no idea (it makes sense why the diffuse, but i thought the geometry value took that into account for specular))
		outputColour += (s + kD * albedoColour / PI_VALUE) * lightPower * nDL;
        //outputColour = fresnel;
    }
    
    //now get ambient and add to output colour
    vec3 ambientFresnel = Fresnel(viewDirection, normalWS, r0);
    //diffuse ambient
    vec3 kD = (1.0 - metallicity) * (1.0 - ambientFresnel);
    vec3 ambientDiffuse = (kD * (irradiance * albedoColour));
    //specular ambient
    vec3 reflectedDir = reflect(-viewDirection, normalWS);
    vec3 prefiltered = textureLod(_PrefilterMap, GetSphericalUV(-reflectedDir), ROUGHNESS_MIP_MAX * roughness).xyz;
    vec2 ambientBRDF = texture(_BRDFMap, vec2(nDV, roughness)).xy; //should return brdf val based on costheta and roughness values
    vec3 ambientSpecular = prefiltered * (ambientFresnel * ambientBRDF.x + ambientBRDF.y);
    outputColour += (ambientDiffuse + ambientSpecular) * ambientOcclusion;

    Colour = vec4(outputColour, 1);
}