cmake_minimum_required(VERSION 3.10)

add_executable(VRRendering main.cpp)
target_link_libraries(VRRendering XRLib::XRLib)
