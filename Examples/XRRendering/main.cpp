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
        .LoadMeshAsync({ "../resources/Duck.glb", "",
                {glm::vec3(-1,0.1,-2), glm::vec3(0,1,0), -40, glm::vec3(0.2,0.2,0.2)} })
    .LoadMeshAsync({ "../resources/viking_room.obj", "../resources/viking_room.png", vikingRoomTransform })
        .LoadMeshAsync({ "../resources/Avocado.glb", "", {glm::vec3(1,0,-2), glm::vec3(0,0.1,0), 0, glm::vec3(5,5,5)} })
        .LoadMeshAsync({ "../resources/Suzanne.gltf", "../resources/Suzanne_BaseColor.png",
                {glm::vec3(2,0.5,-2), glm::vec3(0,0,0), 0, glm::vec3(0.2, 0.2,0.2)} })
    .LoadMeshAsync({"../resources/plane.obj", "", defaultTransform})
        .WaitForAllMeshesToLoad();

    xrLib.Init();
    while (!xrLib.ShouldStop()) {
        xrLib.Run();
    }

    return 0;
}
