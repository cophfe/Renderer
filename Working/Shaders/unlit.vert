#version 460 core
layout (location = 0) in vec3 position;

layout (std140, binding = 0) uniform CameraData
{
    mat4 _ViewProjectionMatrix;		
    mat4 _ProjectionMatrix;			
    mat4 _ViewMatrix;				
	mat4 _InverseViewMatrix;
    vec3 _CameraPosition;
};

uniform mat4 _MVPMatrix;
uniform mat4 _ModelMatrix;
void main()
{
    vec4 worldSpacePosition = _ModelMatrix * vec4(position, 1.0);
    gl_Position = _ViewProjectionMatrix * worldSpacePosition;
}