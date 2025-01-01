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


    XRLib::Entity* leftControllerPtr{ nullptr };
    XRLib::Entity* rightControllerPtr{ nullptr };

    // load meshes
    xrLib.SceneBackend()
        .LoadMeshAsync({ "../resources/Duck.glb", "",
            {glm::vec3(-1,0.1,-2), glm::vec3(0,1,0), -40, glm::vec3(0.5,0.5,0.5)} })
        .LoadMeshAsyncWithBinding({ "../resources/left.glb", "", defaultTransform }, leftControllerPtr).AttachEntityToLeftControllerPose(leftControllerPtr)
        .LoadMeshAsyncWithBinding({ "../resources/right.glb", "", defaultTransform }, rightControllerPtr).AttachEntityToRightcontrollerPose(rightControllerPtr)
        .LoadMeshAsync({ "../resources/sponza.glb", "", defaultTransform })
        .WaitForAllMeshesToLoad();

    XRLib::Transform light;
    light.Translate(glm::vec3(0, 1, 0));
    xrLib.SceneBackend().AddPointLights(light, glm::vec4(1.0f,1.0f,1.0f,1.0f), 0.5);

    xrLib.Init();
    while (!xrLib.ShouldStop()) {
        xrLib.Run();
    }

    return 0;
}
