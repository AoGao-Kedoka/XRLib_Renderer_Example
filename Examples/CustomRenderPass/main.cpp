#include "customPass.h"

int main(){
    XRLib::XRLib xrLib;

    xrLib.SetVersionNumber(1, 0, 0)
        .SetApplicationName("Demo application")
        .EnableValidationLayer()
        .SceneBackend()
        .LoadMeshAsync(
                {.meshPath = "../resources/sponza.glb",
                .transform = {glm::vec3(0,0,0), glm::vec3(0, 0, 0), 0,
                glm::vec3(0.2, 0.2, 0.2)}})
        .WaitForAllMeshesToLoad();

    // prepare custom renderpass
    auto customRenderBehavior = std::make_unique<CustomPass>(xrLib.GetVkCore(), xrLib.SceneBackend(), &xrLib.RenderBackend().RenderPasses, false);
    xrLib.Init(false, std::move(customRenderBehavior));

    while (!xrLib.ShouldStop()){
        xrLib.Run();
    }

    return 0;
}
