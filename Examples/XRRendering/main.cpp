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

    xrLib.SceneBackend()
        .LoadMeshAsync({ .meshPath = "../resources/Duck.glb",
                         .transform = {glm::vec3(-1,0.1,-2), glm::vec3(0,1,0), -40, glm::vec3(0.2,0.2,0.2)} })
        .LoadMeshAsync({ .meshPath = "../resources/viking_room.obj",
                         .transform = vikingRoomTransform,
                         .diffuseTexturePath = "../resources/viking_room.png"})
        .LoadMeshAsync({ .meshPath = "../resources/Suzanne.gltf",
                         .transform = {glm::vec3(2,0.5,-2), glm::vec3(0,0,0), 0, glm::vec3(0.2, 0.2,0.2)},
                         .diffuseTexturePath = "../resources/Suzanne_BaseColor.png"})
        .LoadMeshAsync({.meshPath =  "../resources/sponza.glb", .transform = defaultTransform })
        .LoadMeshAsyncWithBinding({.meshPath = "../resources/left.glb", .transform = defaultTransform}, leftControllerPtr).AttachEntityToLeftControllerPose(leftControllerPtr)
        .LoadMeshAsyncWithBinding({.meshPath = "../resources/right.glb", .transform = defaultTransform}, rightControllerPtr).AttachEntityToRightcontrollerPose(rightControllerPtr)
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
