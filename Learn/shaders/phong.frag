#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inWorldPos;
layout (location = 4) in vec3 inLightPos;
layout (location = 5) in vec3 inCameraPos;

layout (location = 0) out vec4 outFragColor;

void main() {
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	// ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse
	vec3 normal = normalize(inNormal);
	vec3 lightDir = normalize(inLightPos - inWorldPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	float specularStrength = 1.0;
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(inCameraPos - inWorldPos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * inColor;
	outFragColor = vec4(result, 1.0);
}
