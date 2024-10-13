#include "XRLib.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"


XRLib::XRLib        xrLib;
VkDescriptorPool    imguiPool;

void InitImGui() {
    auto core = xrLib.RenderBackend().GetCore();
    VkDescriptorPoolSize pool_sizes[] = {
    {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
    {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
    {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
    {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
    {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
    {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000} };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    vkCreateDescriptorPool(core->GetRenderDevice(), &pool_info, nullptr, &imguiPool);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(XRLib::Graphics::WindowHandler::GetWindow(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = core->GetRenderInstance();
    init_info.PhysicalDevice = core->GetRenderPhysicalDevice();
    init_info.Device = core->GetRenderDevice();
    init_info.Queue = core->GetGraphicsQueue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = imguiPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = 2;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = nullptr;
    init_info.RenderPass = xrLib.RenderBackend().RenderPasses[0]->GetRenderPass().GetVkRenderPass();
    ImGui_ImplVulkan_Init(&init_info);
}

void RegisterImGuiRender() {
    XRLib::EventSystem::Callback callback = []() {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();
        };
    XRLib::EventSystem::RegisterListener(XRLib::Events::XRLIB_EVENT_APPLICATION_PRE_RENDERING, callback);
}

void ImGuiRecord(XRLib::Graphics::CommandBuffer* commandBuffer) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer->GetCommandBuffer());
}

void RegisterImGuiRecord() {
    XRLib::EventSystem::RegisterListener<XRLib::Graphics::CommandBuffer*>(XRLib::Events::XRLIB_EVENT_RENDERER_PRE_SUBMITTING, ImGuiRecord);
}

void ImGuiCleanup() {
    auto core = xrLib.RenderBackend().GetCore();
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(core->GetRenderDevice(), imguiPool, nullptr);
}

int main()
{
    xrLib.SetVersionNumber(1, 0, 0)
        .SetApplicationName("Demo application")
        .EnableValidationLayer();

    xrLib.SceneBackend()
        .LoadMeshAsync({ "../resources/duck.glb", "",
            {glm::vec3(0,0,0), glm::vec3(0,0,0), 0, glm::vec3(0.2,0.2,0.2)} })
        .WaitForAllMeshesToLoad();
    xrLib.Init(false);
    InitImGui();

    RegisterImGuiRender();
    RegisterImGuiRecord();

    while (!xrLib.ShouldStop()) {
        xrLib.Run();
    }

    ImGuiCleanup();
    return 0;
}
