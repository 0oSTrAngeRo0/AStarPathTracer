cmake_minimum_required(VERSION 3.25)
project(PathTracer)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

add_subdirectory(plugins/glfw-3.3.9)
add_subdirectory(plugins/glm-0.9.9.8)
add_subdirectory(plugins/volk-1.3.270)

add_executable(${PROJECT_NAME}
        source/main.cpp
        source/main-window.cpp
        source/vulkan-app.cpp
        source/debugger.cpp
        source/vulkan-app-device.cpp
        source/vulkan-utils.cpp
        source/vulkan-app-instance.cpp
        source/vulkan-app-swap-chain.cpp
        source/file-system.cpp
        source/vulkan-app-graphics-pipeline.cpp
        source/vulkan-app-frame-buffers.cpp
        source/vulkan-app-commands.cpp
        source/vulkan-app-draw-frame.cpp
        source/vulkan-app-ray-tracing.cpp
        source/model-loader.cpp
        source/acceleration-structure.cpp
        source/vk-debugger.cpp
        source/buffer.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE plugins/tiny-obj-loader-1.0.6)

target_link_libraries(${PROJECT_NAME} PRIVATE glm volk)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} PRIVATE glm ${GLM_LIBRARIES})

message(${PROJECT_NAME})
