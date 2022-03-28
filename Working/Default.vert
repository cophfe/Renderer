#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 colour;
layout (location = 3) in vec2 texCoord;

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

void main()
{
	TexCoord = texCoord;

    vec4 worldSpacePosition = _ModelMatrix * vec4(position, 1.0);
    
    Normal = (_ModelMatrix * vec4(normal, 0.0)).xyz;
    gl_Position = _ViewProjectionMatrix * worldSpacePosition;
    FragPos = worldSpacePosition.xyz;
    //worldNormal = (ModelMatrix * vec4(normal, 0.0)).xyz;
	//vertexColour = normal  + 0.5;
	
}