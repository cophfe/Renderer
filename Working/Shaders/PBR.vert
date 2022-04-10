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
    //mat3(transpose(inverse(model))) // matrix for transforming direction vectors, if you want it to work accurately with scaling
};

uniform mat4 _MVPMatrix;
uniform mat4 _ModelMatrix;

struct V2F {
    mat3 matrixTBN;
    vec3 normal;
    vec3 fragPos;
    vec2 texCoord;
};

out V2F vertexOutput;

void main()
{
	vertexOutput.texCoord = texCoord;

    vec4 worldSpacePosition = _ModelMatrix * vec4(position, 1.0);
    vertexOutput.normal = (_ModelMatrix * vec4(normal, 0.0)).xyz;
    vertexOutput.fragPos = worldSpacePosition.xyz;

    gl_Position = _ViewProjectionMatrix * worldSpacePosition;

    //get tbn matrix (IN WORLDSPACE)
    //apparently usually things are transformed into tangent space and not the other way around. in this case, the inverse tbn matrix is just transpose(TBNMatrix)
    vec3 modelN =   normalize(vec3(_ModelMatrix * vec4(normal, 0.0)));
    vec3 modelT =   normalize(vec3(_ModelMatrix * vec4(tangent, 0.0)));
    vec3 modelBiT = normalize(vec3(_ModelMatrix * vec4(bitangent, 0.0)));
    vertexOutput.matrixTBN = mat3(modelT, modelBiT, modelN);
}