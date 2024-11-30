
add_library(tempest_engine
        src/engine.cpp
        src/engine.h
        src/rendering/vulkan/vk_descriptors.cpp
        src/rendering/vulkan/vk_descriptors.h
        src/rendering/vulkan/vk_images.cpp
        src/rendering/vulkan/vk_images.h
        src/rendering/vulkan/vk_initializers.cpp
        src/rendering/vulkan/vk_initializers.h
        src/rendering/vulkan/vk_loader.cpp
        src/rendering/vulkan/vk_loader.h
        src/rendering/vulkan/vk_pipelines.cpp
        src/rendering/vulkan/vk_pipelines.h
        src/rendering/vulkan/vk_types.h
)

target_precompile_headers(tempest_engine PUBLIC
        <optional>
        <vector>
        <memory>
        <string>
        <vector>
        <unordered_map>
        <vulkan/vulkan.hpp>
)

target_include_directories(tempest_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Vulkan REQUIRED)
target_link_libraries(tempest_engine PUBLIC Vulkan::Vulkan)

target_link_libraries(tempest_engine PUBLIC "${Vulkan_LIBRARY}/../slang.lib")

add_subdirectory(third_party/flecs)
target_link_libraries(tempest_engine PUBLIC flecs)

add_subdirectory(third_party/glfw)
target_link_libraries(tempest_engine PUBLIC glfw)

add_subdirectory(third_party/glm)
target_link_libraries(tempest_engine PUBLIC glm::glm)

add_subdirectory(third_party/spdlog)
target_link_libraries(tempest_engine PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

add_subdirectory(third_party/vk-bootstrap)
target_link_libraries(tempest_engine PUBLIC vk-bootstrap::vk-bootstrap)

add_subdirectory(third_party/vma-hpp/VulkanMemoryAllocator)
target_link_libraries(tempest_engine PUBLIC GPUOpen::VulkanMemoryAllocator)

add_subdirectory(third_party/vma-hpp)
target_link_libraries(tempest_engine PUBLIC VulkanMemoryAllocator-Hpp)

add_library(imgui
        third_party/imgui/imgui.h
        third_party/imgui/imgui.cpp
        third_party/imgui/imgui_demo.cpp
        third_party/imgui/imgui_draw.cpp
        third_party/imgui/imgui_widgets.cpp
        third_party/imgui/imgui_tables.cpp
        third_party/imgui/backends/imgui_impl_vulkan.cpp
        third_party/imgui/backends/imgui_impl_glfw.cpp
)
target_include_directories(imgui PUBLIC third_party/imgui third_party/imgui/backends)
target_link_libraries(imgui PRIVATE Vulkan::Vulkan glfw)
target_link_libraries(tempest_engine PUBLIC imgui)
