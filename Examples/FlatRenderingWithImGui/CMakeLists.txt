cmake_minimum_required(VERSION 3.10)


set (VULKAN_DEPS Vulkan::Vulkan)
find_package(Vulkan)
if (NOT Vulkan_FOUND)
    set (VULKAN_DEPS Vulkan::Headers Vulkan::Loader)
endif()

find_package(glfw3)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG master
)
FetchContent_MakeAvailable(imgui)

file(GLOB imgui_SRC ${imgui_SOURCE_DIR}/*.cpp ${imgui_SOURCE_DIR}/*.h)
add_library(
        imgui
        ${imgui_SRC}
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.h
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
 )
target_include_directories(
        imgui
        PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw ${VULKAN_DEPS})

add_executable(FlatRenderingWithImGui main.cpp)
target_link_libraries(FlatRenderingWithImGui PRIVATE XRLib::XRLib imgui)
