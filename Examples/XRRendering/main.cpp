#include "XRLib.h"

int main()
{
    XRLib::XRLib xrLib;

    xrLib.SetVersionNumber(1, 0, 0)
        .SetApplicationName("Demo application")
        //.EnableValidationLayer()

        //Replace this to your own path if non-default openxr runtime want to be used
        .SetCustomOpenXRRuntime("~/Tools/MetaXRSimulator/meta_openxr_simulator.json");

    xrLib.SceneBackend()
        .LoadMeshAsync({ "../resources/duck.glb", "",
            {glm::vec3(0,1,0), glm::vec3(0,0,0), 0, glm::vec3(0.2,0.2,0.2)} })
        .LoadMeshAsync({ "../resources/viking_room.obj", "../resources/viking_room.png",
            {glm::vec3(-0.5,0,0), glm::vec3(0,0,0), 0, glm::vec3(0.2,0.2,0.2)} })
        .LoadMeshAsync({ "../resources/leftHand.glb"}).AttachLeftControllerPose()
        .LoadMeshAsync({ "../resources/rightHand.glb"}).AttachRightControllerPose()
        .WaitForAllMeshesToLoad();

    xrLib.Init();
    while (!xrLib.ShouldStop()) {
        xrLib.Run();
    }

    return 0;
}
