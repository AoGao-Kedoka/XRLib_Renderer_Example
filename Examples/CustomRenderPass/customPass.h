#pragma once
#include "XRLib.h"


class CustomPass : public XRLib::Graphics::VkStandardRB {
    public:
        CustomPass(XRLib::Graphics::VkCore& core, XRLib::Scene& scene, std::vector<std::unique_ptr<XRLib::Graphics::IGraphicsRenderpass>>* renderPasses, bool stereo);
        ~CustomPass() = default;

        void Prepare() override;

    private:
        // buffers
        void InitViewProjectBuffer();
        void InitModelPositionBuffer();
        std::vector<std::shared_ptr<XRLib::Graphics::Image>> InitTexture();

        // descriptor
        std::unique_ptr<XRLib::Graphics::DescriptorSet> InitDescriptorSet();
    private:
        // buffers
        std::shared_ptr<XRLib::Graphics::Buffer> viewProjectBuffer;
        std::shared_ptr<XRLib::Graphics::Buffer> modelPositionBuffer;
        std::vector<std::shared_ptr<XRLib::Graphics::Image>> textures;

        // descriptor layouts and render passes
        std::string vertexShaderPath = "../Examples/CustomRenderPass/shaders/forward.vert";
        std::string fragmentShaderPath = "../Examples/CustomRenderPass/shaders/forward.frag";
};
