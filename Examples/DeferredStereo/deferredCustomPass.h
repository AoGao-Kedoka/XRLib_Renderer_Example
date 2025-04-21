#pragma once
#include "XRLib.h"


/**
* VR Deferred Rendering, basic example
*/
class DeferredCustomPass : public XRLib::Graphics::VkStandardRB
{
public:
	DeferredCustomPass(XRLib::Graphics::VkCore& core, XRLib::Scene& scene,
					   std::vector<std::unique_ptr<XRLib::Graphics::IGraphicsRenderpass>>* renderPasses, bool stereo);
	~DeferredCustomPass() = default;

	void Prepare() override;

	void RecordPass(XRLib::Graphics::CommandBuffer& commandBuffer, XRLib::Graphics::VkGraphicsRenderpass* currentPass, uint8_t currentPassIndex,
					uint32_t& imageIndex) override;

private:
	// render targets
	void CreateRenderTargets();

	// passes
	void CreateOffscreenPass();
	void CreatePresentPass();

	// buffers
	void InitViewProjectBuffer();
	void InitModelPositionBuffer();
	void InitTextures();
	void CreateLightBuffer();

private:
	// buffers
	std::shared_ptr<XRLib::Graphics::Buffer> viewProjectBuffer;
	std::shared_ptr<XRLib::Graphics::Buffer> modelPositionBuffer;
	std::shared_ptr<XRLib::Graphics::Buffer> lightsBuffer;
	std::shared_ptr<XRLib::Graphics::Buffer> lightsCountBuffer;

	struct GBuffer
	{
		std::shared_ptr<XRLib::Graphics::Image> positionTexture;
		std::shared_ptr<XRLib::Graphics::Image> diffuseTexture;
		std::shared_ptr<XRLib::Graphics::Image> normalTexture;
		std::shared_ptr<XRLib::Graphics::Image> metallicRoughnessTexture;
		std::shared_ptr<XRLib::Graphics::Image> emissiveTexture;
	} gBuffer;
	std::vector<std::vector<XRLib::Graphics::Image*>> renderTargets;

	std::vector<std::shared_ptr<XRLib::Graphics::Image>> diffuseTextures;
	std::vector<std::shared_ptr<XRLib::Graphics::Image>> normalTextures;
	std::vector<std::shared_ptr<XRLib::Graphics::Image>> metallicRoughnessTextures;
	std::vector<std::shared_ptr<XRLib::Graphics::Image>> emissiveTextures;


	std::string offscreenVertexShaderPath = "./shaders/offscreen.vert";
	std::string offscreenFragmentShaderPath = "./shaders/offscreen.frag";

	std::string presentVertexShaderPath = "./shaders/present.vert";
	std::string presentFragmentShaderPath = "./shaders/present.frag";
};
