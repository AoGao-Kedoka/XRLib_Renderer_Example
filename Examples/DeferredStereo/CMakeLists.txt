cmake_minimum_required(VERSION 3.10)

add_executable(DeferredStereo main.cpp deferredCustomPass.h deferredCustomPass.cpp)
target_link_libraries(DeferredStereo XRLib::XRLib)

file(GLOB_RECURSE SHADERS "${PROJECT_SOURCE_DIR}/Examples/DeferredStereo/shaders/*")

add_custom_command(TARGET DeferredStereo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:DeferredStereo>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy ${SHADERS} "$<TARGET_FILE_DIR:DeferredStereo>/shaders"
)
