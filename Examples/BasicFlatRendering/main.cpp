#include "XRLib.h"

XRLib::XRLib xrLib;

int main()
{
    xrLib.SetVersionNumber(1, 0, 0)
        .SetApplicationName("Demo application")
        .EnableValidationLayer();

    Transform vikingRoomTransform;
    vikingRoomTransform
        .Translate(glm::vec3(0, 0, 0))
        .Rotate(glm::vec3(1, 0, 0), -90)
        .Rotate(glm::vec3(0, 0, 1), -90)
        .Scale(glm::vec3(0.4, 0.4, 0.4));

    XRLib::Scene::Mesh* avocadoPtr{ nullptr };

    // load meshes
    xrLib.SceneBackend()
        .LoadMeshAsync({ "../resources/Duck.glb", "",
            {glm::vec3(-1,0,0), glm::vec3(0,1,0), -40, glm::vec3(0.2,0.2,0.2)} })
        .LoadMeshAsync({ "../resources/viking_room.obj", "../resources/viking_room.png", vikingRoomTransform })
        .LoadMeshAsync({ "../resources/Avocado.glb", "", {glm::vec3(1,0,0), glm::vec3(0,0,0), 0, glm::vec3(5,5,5)} }).BindToPointer(avocadoPtr)
        .LoadMeshAsync({ "../resources/Suzanne.gltf", "../resources/Suzanne_BaseColor.png",
            {glm::vec3(2,0,0), glm::vec3(0,0,0), 0, glm::vec3(0.2, 0.2,0.2)} })
        .WaitForAllMeshesToLoad();

    // update camera position
    xrLib.SceneBackend().CameraTransform().Translate(glm::vec3(0, -0.3, 0));

    // add lights
    Transform firstLightTransform;
    Transform secondLightTransform;
    secondLightTransform.Translate(glm::vec3(1, 0, 0));
    xrLib.SceneBackend()
        .AddLights({ firstLightTransform, glm::vec4(1.0f,1.0f,1.0f,1.0f), 0.5 })
        .AddLights({ secondLightTransform, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.5 });


    xrLib.Init(false);
    while (!xrLib.ShouldStop()) {

        // rotate the avacado
        avocadoPtr->transform.Rotate(glm::vec3(0, 1, 0), 0.1f);

        xrLib.Run();
    }

    return 0;
}
