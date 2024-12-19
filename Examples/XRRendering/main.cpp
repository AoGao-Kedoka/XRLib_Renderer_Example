#include "XRLib.h"

int main()
{
    XRLib::XRLib xrLib;

    xrLib.SetVersionNumber(1, 0, 0)
        .SetApplicationName("Demo application")
        .EnableValidationLayer()

        //Replace this to your own path if non-default openxr runtime want to be used
        .SetCustomOpenXRRuntime("~/Tools/MetaXRSimulator/meta_openxr_simulator.json");

    XRLib::Transform vikingRoomTransform;
    vikingRoomTransform
        .Translate(glm::vec3(0, 0.1, -2))
        .Rotate(glm::vec3(1, 0, 0), -90)
        .Rotate(glm::vec3(0, 0, 1), -90)
        .Scale(glm::vec3(0.4, 0.4, 0.4));


    XRLib::Transform defaultTransform;


    // load meshes
    xrLib.SceneBackend()
        .LoadMeshAsync({ "../resources/plane.obj", "", defaultTransform })
        .LoadMeshAsync({ "../resources/leftHand.glb", "", defaultTransform }).AttachLeftControllerPose()
        .LoadMeshAsync({ "../resources/rightHand.glb", "", defaultTransform }).AttachRightControllerPose();

    xrLib.Init();
    while (!xrLib.ShouldStop()) {
        xrLib.Run();
    }

    return 0;
}
