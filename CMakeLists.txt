cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

project(tgl)



include_directories(include)

file(GLOB all_SRCS "${PROJECT_SOURCE_DIR}/cpp/*.cpp")
add_executable(tgl ${all_SRCS})

#Add Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(tgl Vulkan::Vulkan)

#Add GLM
find_package(glm REQUIRED)

#Add GLFW
find_package(glfw3 REQUIRED)
target_link_libraries(tgl glfw)

#Add Vk-Bootstrap
include(FetchContent)
FetchContent_Declare(
        fetch_vk_bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
        GIT_TAG        v0.3.1 #suggest using a tag so the library doesn't update whenever new commits are pushed to a branch
)
FetchContent_MakeAvailable(fetch_vk_bootstrap)
target_link_libraries(tgl vk-bootstrap)