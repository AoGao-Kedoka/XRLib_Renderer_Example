#include "deferredCustomPass.h"

/**
* Deferred rendering with a custom pass. For simplicity, resizing is not implemented
*/
int main()
{
	XRLib::XRLib xrLib;

	xrLib.SetVersionNumber(1, 0, 0)
			.SetApplicationName("Demo application")
			.SetWindowProperties(XRLib::Graphics::WindowHandler::WINDOWED, false)
			//.EnableValidationLayer()
			.SetCustomOpenXRRuntime("~/Tools/MetaXRSimulator/meta_openxr_simulator.json")
			.SceneBackend()
			.LoadMeshAsync(
					{.meshPath = "../resources/sponza.glb",
					 .transform = {glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 0,
								   glm::vec3(0.2, 0.2, 0.2)}})
			.WaitForAllMeshesToLoad();
	XRLib::Transform lightTransform;
	lightTransform.Translate(glm::vec3(0, 3, 0));
	xrLib.SceneBackend().AddPointLights(lightTransform, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1);

	// prepare custom renderpass
	auto deferredRenderBehavior = std::make_unique<DeferredCustomPass>(xrLib.GetVkCore(), xrLib.SceneBackend(), &xrLib.RenderBackend().RenderPasses, true);
	xrLib.Init(true, std::move(deferredRenderBehavior));

	if (!xrLib.GetXrCore().IsXRValid()) {
		LOGGER(LOGGER::DEBUG) << "Fallback flat mode not implemented";
		return -1;
	}
	while (!xrLib.ShouldStop()) {
		xrLib.Run();
	}

	return 0;
}
