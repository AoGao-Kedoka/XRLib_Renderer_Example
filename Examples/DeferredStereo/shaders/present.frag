#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_multiview : enable
#extension GL_EXT_nonuniform_qualifier: enable
#extension GL_EXT_debug_printf : enable

struct Light {
    mat4 transform;
    vec4 color;
    float intensity;
};

layout(set = 0, binding = 0) uniform LightsCount {
    int lightsCount;
};

layout(set = 0, binding = 1) readonly buffer Lights {
    Light lights[];
};

layout(set = 0, binding = 2) uniform ViewProj{
    mat4 view;
    mat4 proj;
} vp;

layout(set = 0, binding = 3) uniform sampler2D gBuffers[];

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
    vec3 cameraPos = -vec3(vp.view[3]);
    vec3 worldPos = texture(gBuffers[0], fragTexCoord).rgb;
    vec3 albedo = texture(gBuffers[1], fragTexCoord).rgb;
    vec3 normal = texture(gBuffers[2], fragTexCoord).rgb;

    float ao = texture(gBuffers[3], fragTexCoord).r;
    float metallic = texture(gBuffers[3], fragTexCoord).b;
    float roughness = texture(gBuffers[3], fragTexCoord).g;

    vec3 emissive = texture(gBuffers[4], fragTexCoord).rgb;

    vec3 N = normalize(normal);
    vec3 V = normalize(cameraPos - worldPos);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 Lo = vec3(0.0);


    for (int i = 0; i < lightsCount; ++i) {
        vec3 lightPos = vec3(lights[i].transform[3]);
        vec3 L = normalize(lightPos - worldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPos - worldPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
        vec3 radiance = lights[i].color.rgb * lights[i].intensity * attenuation;
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = numerator / denominator;
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo + emissive;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    outColor = vec4(color, 1);
}
