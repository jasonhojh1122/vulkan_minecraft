#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	mat4 view;
	mat4 proj;
	vec4 cameraPos;
	vec4 lightPos[3];
} ubo;

layout(binding = 1) uniform DynamicUniformObject {
	mat4 model;
} dynamicUbo;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inColor; 

layout (location = 0) flat out vec3 outColor;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	gl_Position = ubo.proj * ubo.view * dynamicUbo.model * vec4(inPos, 1.0);

	vec3 outNormal = mat3(dynamicUbo.model) * inNormal;
	
	vec4 worldPos = dynamicUbo.model * vec4(inPos, 1.0);
	vec3 outWorldPos = worldPos.xyz;
	
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	// ambient
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * lightColor;

	vec3 result = ambient;

	for (int i = 0; i < 3; i++) {
		// diffuse
		vec3 normal = normalize(outNormal);
		vec3 lightDir = normalize(ubo.lightPos[i].xyz - outWorldPos);
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * lightColor;
		result += diffuse;

		// specular
		float specularStrength = 1.0;
		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 viewDir = normalize(ubo.cameraPos.xyz - outWorldPos);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
		vec3 specular = specularStrength * spec * lightColor;
		result += specular;
	}

	outColor = result * inColor;
}
