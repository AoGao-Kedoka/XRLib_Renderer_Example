#include "customPass.h"

CustomPass::CustomPass(XRLib::Graphics::VkCore& core, XRLib::Scene& scene, std::vector<std::unique_ptr<XRLib::Graphics::IGraphicsRenderpass>>* renderPasses, bool stereo) : VkStandardRB(core, scene, renderPasses, stereo)
{
}

void CustomPass::Prepare()
{
	InitViewProjectBuffer();
	InitModelPositionBuffer();
	textures = InitTexture();

	std::vector<std::unique_ptr<XRLib::Graphics::DescriptorSet>> descriptorSets;
	auto descriptorSet = std::make_unique<XRLib::Graphics::DescriptorSet>(core, viewProjectBuffer, modelPositionBuffer, textures);
	descriptorSet->AllocatePushConstant(sizeof(uint32_t));
	descriptorSets.push_back(std::move(descriptorSet));


	auto graphicsRenderPass = std::make_unique<XRLib::Graphics::VkGraphicsRenderpass>(core, false, swapchain->GetSwapchainImages(), true, std::move(descriptorSets), vertexShaderPath, fragmentShaderPath);
	renderPasses->push_back(std::move(graphicsRenderPass));
}

void CustomPass::InitViewProjectBuffer()
{
	auto mainCamera = scene.MainCamera();
	viewProj.view = mainCamera->GetGlobalTransform().GetMatrix();
	viewProj.proj = mainCamera->CameraProjection();
	viewProjectBuffer = std::make_shared<XRLib::Graphics::Buffer>(
			core, sizeof(viewProj), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			static_cast<void*>(&viewProj), false);
}

void CustomPass::InitModelPositionBuffer()
{
	std::vector<glm::mat4> modelPositions(scene.Meshes().size());
	for (int i = 0; i < modelPositions.size(); ++i) {
		modelPositions[i] = scene.Meshes()[i]->GetGlobalTransform().GetMatrix();
	}

	if (modelPositions.empty()) {
		XRLib::Transform tempTransform;
		modelPositions.push_back(tempTransform.GetMatrix());
	}

	modelPositionBuffer =
			std::make_shared<XRLib::Graphics::Buffer>(core, sizeof(glm::mat4) * modelPositions.size(),
													  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
													  static_cast<void*>(modelPositions.data()), false);
}

std::vector<std::shared_ptr<XRLib::Graphics::Image>> CustomPass::InitTexture()
{
	std::vector<std::shared_ptr<XRLib::Graphics::Image>> textures(scene.Meshes().size());
	if (textures.empty()) {
		std::vector<uint8_t> textureData;
		textureData.resize(1 * 1 * 4, 255);
		textures.push_back(std::make_shared<XRLib::Graphics::Image>(core, textureData, 1, 1, 4, VK_FORMAT_R8G8B8A8_SRGB));
		return textures;
	}

	for (int i = 0; i < textures.size(); ++i) {
		textures[i] =
				std::make_shared<XRLib::Graphics::Image>(core, scene.Meshes()[i]->Diffuse.textureData,
														 scene.Meshes()[i]->Diffuse.textureWidth, scene.Meshes()[i]->Diffuse.textureHeight,
														 scene.Meshes()[i]->Diffuse.textureChannels, VK_FORMAT_R8G8B8A8_SRGB);
	}
	return textures;
}
