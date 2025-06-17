cmake_minimum_required(VERSION 3.25)
project(PathTracer)

set(CMAKE_CXX_STANDARD 23)
# add_custom_command(
#     OUTPUT ${PROJECT_SOURCE_DIR}/shaders/compiler-log.log
#     COMMAND ${PROJECT_SOURCE_DIR}/shaders/shader-compiler.bat
#     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/shaders/sources
#     COMMENT "[Shader Compiler] Compiling Shaders..."
# )

# Vulkan
set(VULKAN_LIB_NAME Vulkan::Vulkan)
find_package(Vulkan REQUIRED)

# Vma Hpp
set(VMAHPP_LIB_NAME GPUOpen::VulkanMemoryAllocator VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp)
# add_subdirectory(plugins/VulkanMemoryAllocator-Hpp/Vulkan-Headers)
add_subdirectory(plugins/VulkanMemoryAllocator-Hpp/VulkanMemoryAllocator)
add_subdirectory(plugins/VulkanMemoryAllocator-Hpp)

# GLFW
set(GLFW_LIB_NAME glfw)
set(GLFW_INCLUDE_DIR ${GLFW_SOURCE_DIR}/include)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(plugins/glfw)

# GLM
set(GLM_LIB_NAME glm)
add_subdirectory(plugins/glm)

# EnTT
set(ENTT_LIB_NAME EnTT::EnTT)
add_subdirectory(plugins/entt)

# Cross Guid
set(CROSSGUID_LIB_NAME crossguid)
set(CROSSGUID_TESTS OFF)
add_subdirectory(plugins/crossguid)

# Nlohmann Json
set(NOLHMANJSON_LIB_NAME nlohmann_json::nlohmann_json)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_ImplicitConversions  OFF)
add_subdirectory(plugins/nlohmann-json)

# ImGui
set(IMGUI_LIB_NAME ImGui)
set(IMGUI_DIR plugins/imgui)
set(IMGUI_INCLUDE_DIRS ${IMGUI_DIR} ${IMGUI_DIR}/backends)
add_library(${IMGUI_LIB_NAME})
target_include_directories(${IMGUI_LIB_NAME} PRIVATE
    ${IMGUI_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIR}
)
target_sources(${IMGUI_LIB_NAME} PRIVATE 
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp 
    ${IMGUI_DIR}/imgui.cpp 
    ${IMGUI_DIR}/imgui_draw.cpp 
    ${IMGUI_DIR}/imgui_demo.cpp 
    ${IMGUI_DIR}/imgui_tables.cpp 
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
)

# Nsight-Aftermath
set(NSIGHTAFTERMATH_LIB_NAME ${NSIGHT_AFTERMATH_PROJECT_NAME})
add_subdirectory(plugins/nsight-aftermath)

set(MAIN_EXE_NAME ${PROJECT_NAME})
add_executable(${MAIN_EXE_NAME})
target_sources(${MAIN_EXE_NAME} PRIVATE 
    sources/main.cpp
    sources/acceleration-structure.cpp

    sources/Application/GlfwWindow.cpp
    sources/Application/World.cpp
    sources/Application/RendererPresenter.cpp
    sources/Application/Renderer/RenderContext.cpp
    sources/Application/RendererApplication.cpp
    sources/Application/Renderer/MeshPool.cpp
    sources/Application/Renderer/MaterialPool.cpp
    sources/Application/Renderer/RendererPipeline.cpp
    sources/Application/Materials.cpp
    sources/Application/Renderer/TexturePool.cpp

    sources/Core/RayTracingShaders.cpp
    sources/Core/EngineDebugger.cpp
    sources/Core/Buffer.cpp
    sources/Core/TemporaryCommandBufferPool.cpp
    sources/Core/Image.cpp
    sources/Core/Mesh.cpp
    sources/Core/VulkanUsages.cpp
    sources/Core/Swapchain.cpp
    sources/Core/DeviceContext.cpp
    sources/Core/Surface.cpp

    sources/Editor/EditorUIDrawer.cpp
    sources/Editor/EditorApplication.cpp 
    sources/Editor/EditorUI.cpp
    sources/Editor/EditorRenderContext.cpp
    sources/Editor/EditorSelection.cpp
    sources/Editor/UI/Inspectors/ResourceInspector.cpp
    sources/Editor/UI/ImGuiFileDialog.cpp
    sources/Editor/UI/ResourcesPanel.cpp
    sources/Editor/UI/TreeViewUtilities.cpp
    sources/Editor/UI/HierarchiesPanel.cpp
    sources/Editor/UI/Inspectors/ComponentInspector.cpp
    sources/Editor/UI/ViewportPanel.cpp
    
    sources/Engine/Resources/Resource.cpp
    sources/Engine/Resources/ResourcesManager.cpp
    sources/Engine/Resources/ResourceData.cpp
    sources/Engine/Resources/ObjResourceData.cpp
    sources/Engine/Resources/MeshLoader/MikkTSpace/mikktspace.c
    sources/Engine/Resources/MeshLoader/MikkTSpace/MikkTSpaceTangentGenerator.cpp
    sources/Engine/ShaderHostBuffer.cpp
    sources/Engine/Resources/TextureResourceData.cpp
    sources/Engine/Components/ComponentInspectors.cpp
    sources/Engine/Json/Components.cpp
)
target_include_directories(${MAIN_EXE_NAME} PRIVATE
    ${IMGUI_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
    plugins/tiny-obj-loader-1.0.6
    includes
    sources
)
target_link_libraries(${MAIN_EXE_NAME} PRIVATE 
    ${IMGUI_LIB_NAME}
    ${ENTT_LIB_NAME}
    ${GLFW_LIB_NAME}
    ${VULKAN_LIB_NAME}
    ${GLM_LIB_NAME}
    ${VMAHPP_LIB_NAME}
    ${CROSSGUID_LIB_NAME}
    ${NOLHMANJSON_LIB_NAME}
    ${NSIGHTAFTERMATH_LIB_NAME}
)
