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
