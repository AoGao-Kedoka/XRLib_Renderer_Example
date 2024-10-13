#include "XRLib.h"

XRLib::XRLib xrLib;

int main()
{
    xrLib.SetVersionNumber(1, 0, 0)
        .SetApplicationName("Demo application")
        .EnableValidationLayer();

    Transform vikingRoomTransform;
    vikingRoomTransform
        .Translate(glm::vec3(-0.5, -0.5, 0))
        .Rotate(glm::vec3(1, 0, 0), -90)
        .Rotate(glm::vec3(0, 0, 1), -90)
        .Scale(glm::vec3(0.5, 0.5, 0.5));

    xrLib.SceneBackend()
        .LoadMeshAsync({ "../resources/Duck.glb", "",
            {glm::vec3(0,0.5,0), glm::vec3(0,1,0), -40, glm::vec3(0.2,0.2,0.2)} })
        .LoadMeshAsync({ "../resources/viking_room.obj", "../resources/viking_room.png", vikingRoomTransform })
        .LoadMeshAsync({ "../resources/Avocado.glb", "", {glm::vec3(0,0,0), glm::vec3(0,0,0), 0, glm::vec3(5,5,5)} })
        .WaitForAllMeshesToLoad();

    xrLib.Init(false);
    while (!xrLib.ShouldStop()) {

        // rotate the avacado
        xrLib.SceneBackend().Meshes()[2].transform.Rotate(glm::vec3(0, 1, 0), 0.1f);

        xrLib.Run();
    }

    return 0;
}
