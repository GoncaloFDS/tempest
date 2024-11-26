
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

add_subdirectory(third_party/flecs)
target_link_libraries(tempest_engine PRIVATE flecs)

add_subdirectory(third_party/glfw)
target_link_libraries(tempest_engine PRIVATE glfw)

add_subdirectory(third_party/glm)
target_link_libraries(tempest_engine PRIVATE glm::glm)

add_subdirectory(third_party/spdlog)
target_link_libraries(tempest_engine PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
target_include_directories(tempest_engine PUBLIC third_party/spdlog/include)

add_subdirectory(third_party/vk-bootstrap)
target_link_libraries(tempest_engine PRIVATE vk-bootstrap::vk-bootstrap)

add_subdirectory(third_party/vma)
target_link_libraries(tempest_engine PRIVATE GPUOpen::VulkanMemoryAllocator)


