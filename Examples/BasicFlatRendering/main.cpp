#include "XRLib.h"

XRLib::XRLib xrLib;

int main()
{
    xrLib.SetVersionNumber(1, 0, 0)
        .SetApplicationName("Demo application")
        .EnableValidationLayer();

    XRLib::Transform vikingRoomTransform;
    vikingRoomTransform
        .Translate(glm::vec3(0, 0.1, -2))
        .Rotate(glm::vec3(1, 0, 0), -90)
        .Rotate(glm::vec3(0, 0, 1), -90)
        .Scale(glm::vec3(0.4, 0.4, 0.4));

    XRLib::Transform defaultTransform;

    XRLib::Entity* avocadoPtr{ nullptr };

    // load meshes
    xrLib.SceneBackend()
        .LoadMeshAsync({ "../resources/Duck.glb", "",
            {glm::vec3(-1,0.1,-2), glm::vec3(0,1,0), -40, glm::vec3(0.2,0.2,0.2)} })
        .LoadMeshAsync({ "../resources/viking_room.obj", "../resources/viking_room.png", vikingRoomTransform })
        .LoadMeshAsyncWithBinding({ "../resources/Avocado.glb", "", {glm::vec3(1,0,-2), glm::vec3(0,0.1,0), 0, glm::vec3(5,5,5)}}, avocadoPtr)
        .LoadMeshAsync({ "../resources/Suzanne.gltf", "../resources/Suzanne_BaseColor.png",
            {glm::vec3(2,0.5,-2), glm::vec3(0,0,0), 0, glm::vec3(0.2, 0.2,0.2)} })
        .LoadMeshAsync({"../resources/sponza.glb", "", defaultTransform})
        .WaitForAllMeshesToLoad();

    // update camera position
    xrLib.SceneBackend().MainCamera()->GetLocalTransform().Translate(glm::vec3(0, -1.3, 0));

    // add lights
    XRLib::Transform lightTransform;
    lightTransform.Translate(glm::vec3(0, 1, 0));
    xrLib.SceneBackend().AddPointLights(lightTransform, glm::vec4(1.0f,1.0f,1.0f,1.0f), 0.5, avocadoPtr);

    xrLib.Init(false);

    LOGGER(LOGGER::DEBUG) << xrLib.SceneBackend();

    while (!xrLib.ShouldStop()) {

        // rotate the avacado
        if (avocadoPtr)
            avocadoPtr->GetLocalTransform().Rotate(glm::vec3(0, 1, 0), 0.1f);

        xrLib.Run();
    }

    return 0;
}
