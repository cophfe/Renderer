#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent;
layout (location = 4) in vec2 texCoord;

layout (std140, binding = 0) uniform CameraData
{
    mat4 _ViewProjectionMatrix;		//0
    mat4 _ProjectionMatrix;			//64
    mat4 _ViewMatrix;				//128
	mat4 _InverseViewMatrix;
    vec3 _CameraPosition;
    //mat3(transpose(inverse(model))) // matrix for normals
};

layout (std140, binding = 1) uniform Timing
{
    float _Time;						//0
	float _DeltaTime;				//4
};

uniform mat4 _MVPMatrix;
uniform mat4 _ModelMatrix;

out vec2 TexCoord; 
out vec3 FragPos;
out vec3 Normal;
out mat3 TBNMatrix; 
//apparently usually things are transformed into tangent space and not the other way around. in this case, the inverse tbn matrix is just transpose(TBNMatrix)

void main()
{
	TexCoord = texCoord;

    vec4 worldSpacePosition = _ModelMatrix * vec4(position, 1.0);
    
    Normal = (_ModelMatrix * vec4(normal, 0.0)).xyz;
	//Normal = normal;
    gl_Position = _ViewProjectionMatrix * worldSpacePosition;
    FragPos = worldSpacePosition.xyz;


    //get tbn matrix (IN WORLDSPACE)
    vec3 ModelN =   normalize(vec3(_ModelMatrix * vec4(normal, 0.0)));
    vec3 ModelT =   normalize(vec3(_ModelMatrix * vec4(tangent, 0.0)));
    vec3 ModelBiT = normalize(vec3(_ModelMatrix * vec4(bitangent, 0.0)));
    TBNMatrix = mat3(ModelT, ModelBiT, ModelN);
}