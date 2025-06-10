cmake_minimum_required(VERSION 4.0.2)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
project(PathTracer)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_COMPILER_IMPORT_STD ON)

# add_custom_command(
#     OUTPUT ${PROJECT_SOURCE_DIR}/shaders/compiler-log.log
#     COMMAND ${PROJECT_SOURCE_DIR}/shaders/shader-compiler.bat
#     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/shaders/sources
#     COMMENT "[Shader Compiler] Compiling Shaders..."
# )

# Vulkan
find_package(Vulkan REQUIRED)
if(${Vulkan_VERSION} VERSION_LESS "1.3.256")
    message(FATAL_ERROR "Minimum required Vulkan version for C++ modules is 1.3.256. " "Found ${Vulkan_VERSION}.")
endif()
add_library(VulkanHppModule)
target_sources(VulkanHppModule PUBLIC
    FILE_SET CXX_MODULES
    BASE_DIRS ${Vulkan_INCLUDE_DIR}
    FILES ${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm
)
target_compile_features(VulkanHppModule PRIVATE cxx_std_23)
set_target_properties(VulkanHppModule PROPERTIES CXX_MODULE_STD ON) 
target_compile_definitions(VulkanHppModule PRIVATE
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    # VULKAN_HPP_NO_EXCEPTIONS
    # VULKAN_HPP_NO_SMART_HANDLE
    # VULKAN_HPP_NO_CONSTRUCTORS
)
target_link_libraries(VulkanHppModule PRIVATE Vulkan::Vulkan)

# Vulkan Memory Allocator Hpp
add_subdirectory(plugins/VulkanMemoryAllocator-Hpp/VulkanMemoryAllocator)
add_subdirectory(plugins/VulkanMemoryAllocator-Hpp)
# add_library(VulkanMemoryAllocatorHppModule)
# target_sources(VulkanMemoryAllocatorHppModule PUBLIC
#     FILE_SET CXX_MODULES
#     BASE_DIRS ${PROJECT_SOURCE_DIR}/plugins/VulkanMemoryAllocator-Hpp/src
#     FILES ${PROJECT_SOURCE_DIR}/plugins/VulkanMemoryAllocator-Hpp/src/vk_mem_alloc.cppm
# )
# target_compile_features(VulkanMemoryAllocatorHppModule PUBLIC cxx_std_23)
# set_target_properties(VulkanMemoryAllocatorHppModule PROPERTIES CXX_MODULE_STD ON)
# target_link_libraries(VulkanMemoryAllocatorHppModule PRIVATE VulkanHppModule GPUOpen::VulkanMemoryAllocator VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp)
# target_compile_definitions(VulkanMemoryAllocatorHppModule PRIVATE VMA_ENABLE_VULKAN_HPP_MODULE)


set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)

add_subdirectory(plugins/glfw)
add_subdirectory(plugins/glm)
add_subdirectory(plugins/entt)
add_subdirectory(plugins/crossguid)
add_subdirectory(plugins/nsight-aftermath)

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
    ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
)

add_library(Core)
target_sources(Core PRIVATE
    FILE_SET CXX_MODULES
    BASE_DIRS sources/Core
    FILES sources/Core/Core.cppm sources/Core/Buffer.cppm sources/Core/DeviceContext.cppm
)
target_include_directories(Core PRIVATE includes)
target_include_directories(Core PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(Core PRIVATE VulkanHppModule)
target_link_libraries(Core PRIVATE GPUOpen::VulkanMemoryAllocator VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp)
target_compile_features(Core PUBLIC cxx_std_23)
set_target_properties(Core PROPERTIES CXX_MODULE_STD ON) 

add_executable(${PROJECT_NAME}
    ${IMGUI_SOURCE}
    
    sources/main.cpp
    sources/acceleration-structure.cpp

    sources/Application/GlfwWindow.cpp
    sources/Application/World.cpp
    sources/Application/Renderer/Renderer.cpp
    sources/Application/Renderer/RenderContext.cpp
    sources/Application/Renderer/RendererApplication.cpp
    sources/Application/Renderer/MeshPool.cpp
    sources/Application/Renderer/MaterialPool.cpp
    sources/Application/Renderer/RendererPipeline.cpp
    sources/Application/Materials.cpp
    sources/Application/Renderer/TexturePool.cpp

    sources/Core/RayTracingShaders.cpp
    sources/Core/EngineDebugger.cpp
    sources/Core/TemporaryCommandBufferPool.cpp
    sources/Core/Image.cpp
    sources/Core/Mesh.cpp
    sources/Core/VulkanUsages.cpp
    sources/Core/Swapchain.cpp
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
    
    sources/Engine/Resources/Resource.cpp
    sources/Engine/Resources/ResourcesManager.cpp
    sources/Engine/Resources/ResourceData.cpp
    sources/Engine/Resources/MeshLoader/ObjMeshLoader.cpp
    sources/Engine/Resources/MeshLoader/MikkTSpace/mikktspace.c
    sources/Engine/Resources/MeshLoader/MikkTSpace/MikkTSpaceTangentGenerator.cpp
    sources/Engine/Resources/MeshLoader/MeshResourceUtilities.cpp
    sources/Engine/ShaderHostBuffer.cpp
    sources/Engine/Resources/TextureLoader/StbImageUtilities.cpp
    sources/Engine/Components/ComponentInspectors.cpp
    sources/Engine/Json/Components.cpp
    
)


# target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE plugins/tiny-obj-loader-1.0.6)
target_include_directories(${PROJECT_NAME} PRIVATE includes)
target_include_directories(${PROJECT_NAME} PRIVATE sources)
target_include_directories(${PROJECT_NAME} PRIVATE ${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

target_link_libraries(${PROJECT_NAME} PRIVATE Core)
target_link_libraries(${PROJECT_NAME} PRIVATE EnTT::EnTT)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE VulkanHppModule)
target_link_libraries(${PROJECT_NAME} PRIVATE GPUOpen::VulkanMemoryAllocator VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp)
target_link_libraries(${PROJECT_NAME} PRIVATE glm ${GLM_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE crossguid)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PRIVATE NsightAftermath)
