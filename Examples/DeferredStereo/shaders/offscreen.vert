#version 450
#extension GL_ARB_separate_shader_objects : enable
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

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragWorldPos;
layout(location = 3) out vec3 cameraPos;

void main() {
    vec4 worldPos = models[modelIndex] * vec4(inPosition, 1.0);
    gl_Position = vp.proj * vp.view * worldPos;
    mat3 normalMatrix = transpose(inverse(mat3(models[modelIndex])));
    fragNormal = normalize(normalMatrix * inNormal);
    fragTexCoord = inTexCoord;
    fragWorldPos = worldPos.xyz;
    cameraPos = -vec3(vp.view[3]);
}