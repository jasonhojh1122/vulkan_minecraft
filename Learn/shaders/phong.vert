#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	vec4 cameraPos;
	vec4 lightPos[3];
} ubo;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inColor; 

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec3 outWorldPos;
layout (location = 4) out vec3 outLightPos[3];
layout (location = 7) out vec3 outCameraPos;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	outNormal = inNormal;
	outColor = inColor;
	outUV = inUV;
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPos, 1.0);

	outNormal = mat3(ubo.model) * inNormal;
	
	vec4 worldPos = ubo.model * vec4(inPos, 1.0);
	outWorldPos = worldPos.xyz;

	for (int i = 0; i < 3; i++)
		outLightPos[i] = ubo.lightPos[i].xyz;

	outCameraPos = ubo.cameraPos.xyz;
}