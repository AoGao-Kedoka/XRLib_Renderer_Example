#pragma once
#include "XRLib.h"


/**
* Custom render behavior, simply override functions in vulkan standard rendering behavior
*/
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

    private:
        // buffers
        std::shared_ptr<XRLib::Graphics::Buffer> viewProjectBuffer;
        std::shared_ptr<XRLib::Graphics::Buffer> modelPositionBuffer;
        std::vector<std::shared_ptr<XRLib::Graphics::Image>> textures;

        // descriptor layouts and render passes
        std::string vertexShaderPath = "./shaders/forward_no_light.vert";
        std::string fragmentShaderPath = "./shaders/forward_no_light.frag";
};
