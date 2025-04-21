#include "deferredCustomPass.h"

DeferredCustomPass::DeferredCustomPass(XRLib::Graphics::VkCore& core, XRLib::Scene& scene, std::vector<std::unique_ptr<XRLib::Graphics::IGraphicsRenderpass>>* renderPasses, bool stereo) : VkStandardRB(core, scene, renderPasses, stereo)
{
}

////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////
void DeferredCustomPass::Prepare()
{
	CreateOffscreenPass();
	CreatePresentPass();
}

void DeferredCustomPass::CreateOffscreenPass()
{
	// Initialize buffers and render targets
	InitViewProjectBuffer();
	InitModelPositionBuffer();

	InitTextures();
	CreateRenderTargets();

	renderTargets.resize(swapchain->FramesInFlight());
	for (auto& renderTarget: renderTargets) {
		renderTarget.push_back(gBuffer.positionTexture.get());
		renderTarget.push_back(gBuffer.diffuseTexture.get());
		renderTarget.push_back(gBuffer.normalTexture.get());
		renderTarget.push_back(gBuffer.metallicRoughnessTexture.get());
		renderTarget.push_back(gBuffer.emissiveTexture.get());
	}

	// Initialize descriptorsets
	std::vector<std::unique_ptr<XRLib::Graphics::DescriptorSet>> descriptorSets;
	auto descriptorSet = std::make_unique<XRLib::Graphics::DescriptorSet>(core, viewProjectBuffer, modelPositionBuffer, diffuseTextures, normalTextures, metallicRoughnessTextures, emissiveTextures);
	descriptorSet->AllocatePushConstant(sizeof(uint32_t));
	descriptorSets.push_back(std::move(descriptorSet));

	// Initialize geometry pass, g-buffer as render targets
	auto graphicsRenderPass = std::make_unique<XRLib::Graphics::VkGraphicsRenderpass>(core, stereo, renderTargets, false, std::move(descriptorSets), offscreenVertexShaderPath, offscreenFragmentShaderPath);
	renderPasses->push_back(std::move(graphicsRenderPass));
}

void DeferredCustomPass::CreatePresentPass()
{
	// Initialize buffers
	CreateLightBuffer();

	// Initialize descriptorsets
	std::vector<std::unique_ptr<XRLib::Graphics::DescriptorSet>> descriptorSets;
	std::vector<std::shared_ptr<XRLib::Graphics::Image>> gBuffers{gBuffer.positionTexture, gBuffer.diffuseTexture, gBuffer.normalTexture, gBuffer.metallicRoughnessTexture, gBuffer.emissiveTexture};
	auto descriptorSet = std::make_unique<XRLib::Graphics::DescriptorSet>(core, lightsCountBuffer, lightsBuffer, viewProjectBuffer, gBuffers);
	descriptorSets.push_back(std::move(descriptorSet));

	// Initialize lighting pass
	auto presentPass = std::make_unique<XRLib::Graphics::VkGraphicsRenderpass>(core, stereo, swapchain->GetSwapchainImages(), true, std::move(descriptorSets), presentVertexShaderPath, presentFragmentShaderPath);
	renderPasses->push_back(std::move(presentPass));
}

////////////////////////////////////////////////////
// Recording passes
////////////////////////////////////////////////////
void DeferredCustomPass::RecordPass(XRLib::Graphics::CommandBuffer& commandBuffer, XRLib::Graphics::VkGraphicsRenderpass* currentPass, uint8_t currentPassIndex, uint32_t& imageIndex)
{
	commandBuffer.StartPass(*currentPass, imageIndex).BindDescriptorSets(*currentPass, 0);
	switch (currentPassIndex) {
		// draw everything to the gbuffer
		case 0: {
			for (uint32_t i = 0; i < scene.Meshes().size(); ++i) {
				commandBuffer.PushConstant(*currentPass, sizeof(uint32_t), &i);
				if (!vertexBuffers.empty() && !indexBuffers.empty() && vertexBuffers[i] != nullptr &&
					indexBuffers[i] != nullptr) {
					commandBuffer.BindVertexBuffer(0, {vertexBuffers[i]->GetBuffer()}, {0})
							.BindIndexBuffer(indexBuffers[i]->GetBuffer(), 0);
				}

				commandBuffer.DrawIndexed(scene.Meshes()[i]->GetIndices().size(), 1, 0, 0, 0);
			}
			break;
		}

		// draw the gbuffer
		case 1: {
			commandBuffer.DrawIndexed(scene.Meshes()[0]->GetIndices().size(), 1, 0, 0, 0);
			break;
		}
	}
	commandBuffer.EndPass();
}

////////////////////////////////////////////////////
// Prepare resources
////////////////////////////////////////////////////
void DeferredCustomPass::InitViewProjectBuffer()
{
	viewProjectBuffer = std::make_shared<XRLib::Graphics::Buffer>(
			core, sizeof(viewProjStereo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			static_cast<void*>(&viewProjStereo), false);

	XRLib::EventSystem::Callback<std::vector<glm::mat4>, std::vector<glm::mat4>> bufferCamUpdateCallback =
			[&](std::vector<glm::mat4> views, std::vector<glm::mat4> projs) {
				if (views.size() != 2 || projs.size() != 2) {
					XRLib::Util::ErrorPopup("Unknown view size, please use custom shader");
					return;
				}

				for (int i = 0; i < 2; ++i) {
					viewProjStereo.views[i] = views[i];
					viewProjStereo.projs[i] = projs[i];
				}

				viewProjectBuffer->UpdateBuffer(sizeof(viewProjStereo), static_cast<void*>(&viewProjStereo));
			};

	XRLib::EventSystem::RegisterListener(XRLib::Events::XRLIB_EVENT_HEAD_MOVEMENT, bufferCamUpdateCallback);
}

void DeferredCustomPass::InitModelPositionBuffer()
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

void DeferredCustomPass::InitTextures()
{
	diffuseTextures.resize(scene.Meshes().size());
	normalTextures.resize(scene.Meshes().size());
	metallicRoughnessTextures.resize(scene.Meshes().size());
	emissiveTextures.resize(scene.Meshes().size());

	for (int i = 0; i < scene.Meshes().size(); ++i) {
		diffuseTextures[i] =
				std::make_shared<XRLib::Graphics::Image>(core, scene.Meshes()[i]->Diffuse.textureData,
														 scene.Meshes()[i]->Diffuse.textureWidth, scene.Meshes()[i]->Diffuse.textureHeight,
														 scene.Meshes()[i]->Diffuse.textureChannels, VK_FORMAT_R8G8B8A8_SRGB);
		normalTextures[i] =
				std::make_shared<XRLib::Graphics::Image>(core, scene.Meshes()[i]->Normal.textureData,
														 scene.Meshes()[i]->Normal.textureWidth, scene.Meshes()[i]->Normal.textureHeight,
														 scene.Meshes()[i]->Normal.textureChannels, VK_FORMAT_R8G8B8A8_SRGB);
		metallicRoughnessTextures[i] =
				std::make_shared<XRLib::Graphics::Image>(core, scene.Meshes()[i]->MetallicRoughness.textureData,
														 scene.Meshes()[i]->MetallicRoughness.textureWidth, scene.Meshes()[i]->MetallicRoughness.textureHeight,
														 scene.Meshes()[i]->MetallicRoughness.textureChannels, VK_FORMAT_R8G8B8A8_SRGB);
		emissiveTextures[i] =
				std::make_shared<XRLib::Graphics::Image>(core, scene.Meshes()[i]->Emissive.textureData,
														 scene.Meshes()[i]->Emissive.textureWidth, scene.Meshes()[i]->Emissive.textureHeight,
														 scene.Meshes()[i]->Emissive.textureChannels, VK_FORMAT_R8G8B8A8_SRGB);
	}
}

void DeferredCustomPass::CreateLightBuffer()
{
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	struct PointLightData
	{
		XRLib::Transform transform;
		glm::vec4 color;
		float intensity;
	};

	std::vector<PointLightData> pointLightDataBuffer(scene.PointLights().size());
	for (size_t i = 0; i < pointLightDataBuffer.size(); ++i) {
		pointLightDataBuffer[i].transform = scene.PointLights()[i]->GetGlobalTransform();
		pointLightDataBuffer[i].color = scene.PointLights()[i]->GetColor();
		pointLightDataBuffer[i].intensity = scene.PointLights()[i]->GetIntensity();
	}

	lightsBuffer = std::make_unique<XRLib::Graphics::Buffer>(core, sizeof(PointLightData) * scene.PointLights().size(), usage,
															 static_cast<void*>(pointLightDataBuffer.data()), false);
	usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	int lightsCount = scene.PointLights().size();
	lightsCountBuffer =
			std::make_shared<XRLib::Graphics::Buffer>(core, sizeof(int), usage, static_cast<void*>(&lightsCount), false);
}

void DeferredCustomPass::CreateRenderTargets()
{
	auto [width, height] = XRLib::Graphics::WindowHandler::GetFrameBufferSize();
	gBuffer.positionTexture = std::make_shared<XRLib::Graphics::Image>(core, width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
																	   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 2);
	gBuffer.diffuseTexture = std::make_shared<XRLib::Graphics::Image>(core, width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
																	  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 2);
	gBuffer.normalTexture = std::make_shared<XRLib::Graphics::Image>(core, width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
																	 VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 2);
	gBuffer.metallicRoughnessTexture = std::make_shared<XRLib::Graphics::Image>(core, width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
																				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 2);
	gBuffer.emissiveTexture = std::make_shared<XRLib::Graphics::Image>(core, width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
																	   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 2);
}

