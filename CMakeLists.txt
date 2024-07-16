cmake_minimum_required(VERSION 3.25)
project(PathTracer)

set(CMAKE_CXX_STANDARD 23)
# add_custom_command(
#     OUTPUT ${PROJECT_SOURCE_DIR}/shaders/compiler-log.log
#     COMMAND ${PROJECT_SOURCE_DIR}/shaders/shader-compiler.bat
#     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/shaders/sources
#     COMMENT "[Shader Compiler] Compiling Shaders..."
# )

find_package(Vulkan REQUIRED)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)

add_subdirectory(plugins/glfw)
add_subdirectory(plugins/glm)
add_subdirectory(plugins/VulkanMemoryAllocator-Hpp)
add_subdirectory(plugins/entt)
add_subdirectory(plugins/crossguid)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_ImplicitConversions  OFF)
add_subdirectory(plugins/nlohmann-json)

set(IMGUI_DIR plugins/imgui)
set(IMGUI_SOURCE 
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp 
    ${IMGUI_DIR}/imgui.cpp 
    ${IMGUI_DIR}/imgui_draw.cpp 
    ${IMGUI_DIR}/imgui_demo.cpp 
    ${IMGUI_DIR}/imgui_tables.cpp 
    ${IMGUI_DIR}/imgui_widgets.cpp
)
message("[AStar] ${IMGUI_DIR}")

add_executable(${PROJECT_NAME}
    ${IMGUI_SOURCE}
    
    sources/main.cpp
    sources/acceleration-structure.cpp

    sources/Application/GlfwWindow.cpp
    sources/Application/Systems.cpp
    sources/Application/Renderer/Renderer.cpp
    sources/Application/Renderer/RenderContext.cpp
    sources/Application/Renderer/RendererApplication.cpp

    sources/Core/RayTracingShaderBindingTable.cpp
    sources/Core/EngineDebugger.cpp
    sources/Core/Buffer.cpp
    sources/Core/TemporaryCommandBufferPool.cpp
    sources/Core/Image.cpp
    sources/Core/Mesh.cpp
    sources/Core/ModelLoader/ObjLoader.cpp
    sources/Core/VulkanUsages.cpp
    sources/Core/Swapchain.cpp
    sources/Core/DeviceContext.cpp

    sources/Editor/EditorUIDrawer.cpp
    sources/Editor/EditorApplication.cpp 
    sources/Editor/EditorUI.cpp
    sources/Editor/EditorRenderContext.cpp
    sources/Editor/EditorSelection.cpp
    sources/Editor/UI/Inspectors/ResourceInspector.cpp
    sources/Editor/UI/ImGuiFileDialog.cpp
    sources/Editor/UI/ResourcesPanel.cpp
    sources/Editor/UI/FileBrowser.cpp

    sources/Engine/Resources/Resource.cpp
    sources/Engine/Resources/ResourcesManager.cpp

)

target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE plugins/tiny-obj-loader-1.0.6)
target_include_directories(${PROJECT_NAME} PRIVATE includes)
target_include_directories(${PROJECT_NAME} PRIVATE sources)
target_include_directories(${PROJECT_NAME} PRIVATE ${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

target_link_libraries(${PROJECT_NAME} PRIVATE EnTT::EnTT)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} PRIVATE glm ${GLM_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE VulkanMemoryAllocator-Hpp)
target_link_libraries(${PROJECT_NAME} PRIVATE crossguid)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
