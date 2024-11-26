#pragma once

#include <array>
#include <deque>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <vector>

#include "spdlog/spdlog.h"
#include "vk_mem_alloc.h"
#include "vulkan/vk_enum_string_helper.h"
#include "vulkan/vulkan.hpp"

#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"

#define VK_CHECK(x)                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        VkResult err = x;                                                                                              \
        if (err)                                                                                                       \
        {                                                                                                              \
            spdlog::error("Detected Vulkan error: {}", string_VkResult(err));                                          \
            abort();                                                                                                   \
        }                                                                                                              \
    } while (0)

struct AllocatedImage
{
    vk::Image image;
    vk::ImageView imageView;
    VmaAllocation allocation;
    vk::Extent3D imageExtent;
    vk::Format imageFormat;
};