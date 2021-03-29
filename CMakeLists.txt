cmake_minimum_required(VERSION 3.17)
project(tgl)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

file(GLOB all_SRCS "${PROJECT_SOURCE_DIR}/cpp/*.cpp")
add_executable(tgl ${all_SRCS})

#Add Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(tgl Vulkan::Vulkan)

#Add GLM
find_package(glm REQUIRED)

#Add GLFW
add_subdirectory(thirdparty/glfw-3.3.2)
target_link_libraries(tgl glfw)