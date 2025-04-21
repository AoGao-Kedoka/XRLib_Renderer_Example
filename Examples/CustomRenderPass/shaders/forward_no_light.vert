#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_debug_printf : enable

layout(set = 0, binding = 0) uniform ViewProj{
	mat4 view;
	mat4 proj;
} vp;

layout(set = 0, binding = 1) readonly buffer ModelMatrices {
	mat4 models[];
};

layout(push_constant) uniform PushConstants {
	uint modelIndex;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;


void main() {
	gl_Position = vp.proj * vp.view * models[modelIndex] * vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
}
