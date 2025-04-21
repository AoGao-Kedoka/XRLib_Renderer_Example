#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_multiview : enable
#extension GL_EXT_nonuniform_qualifier: enable

layout(set = 0, binding = 2) uniform sampler2D diffuseSamplers[];
layout(set = 0, binding = 3) uniform sampler2D normalSamplers[];
layout(set = 0, binding = 4) uniform sampler2D metallicRoughnessSampler[];
layout(set = 0, binding = 5) uniform sampler2D emissiveSamplers[];


layout(push_constant) uniform PushConstants {
    uint modelIndex;
};

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragWorldPos;
layout(location = 3) in vec3 cameraPos;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outDiffuse;
layout(location = 2) out vec4 outNormal;
layout(location = 3) out vec4 outRoughnessMetallic;
layout(location = 4) out vec4 outEmissive;


void main() {
    outPosition = vec4(fragWorldPos, 1.0);
    outDiffuse = texture(diffuseSamplers[modelIndex], fragTexCoord);
    outNormal = vec4(fragNormal, 1.0);
    outRoughnessMetallic = texture(metallicRoughnessSampler[modelIndex], fragTexCoord);
    outEmissive = texture(emissiveSamplers[modelIndex], fragTexCoord);
}