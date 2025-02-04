cmake_minimum_required(VERSION 3.10)

add_executable(CustomRenderPass main.cpp customPass.h customPass.cpp)
target_link_libraries(CustomRenderPass XRLib::XRLib)

file(GLOB_RECURSE SHADERS "${PROJECT_SOURCE_DIR}/Examples/CustomRenderPass/shaders/*")

add_custom_command(TARGET CustomRenderPass POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:CustomRenderPass>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy ${SHADERS} "$<TARGET_FILE_DIR:CustomRenderPass>/shaders"
)
