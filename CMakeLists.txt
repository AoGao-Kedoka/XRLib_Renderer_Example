cmake_minimum_required(VERSION 3.15)
project(XRLib_Example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
	XRLib
	# SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../XRLib
	GIT_REPOSITORY https://github.com/AoGao-Kedoka/XRLib_Renderer.git
	GIT_TAG main
 )
FetchContent_MakeAvailable(XRLib)


add_subdirectory(Examples/BasicFlatRendering)
add_subdirectory(Examples/XRRendering)
add_subdirectory(Examples/FlatRenderingWithImGui)
add_subdirectory(Examples/CustomRenderPass)
add_subdirectory(Examples/DeferredStereo)

add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/Examples/resources/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/resources"
        "${PROJECT_BINARY_DIR}/Examples/resources"
)
