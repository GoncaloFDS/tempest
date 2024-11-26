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
#include "vulkan/vk_enum_string_helper.h"
#include "vulkan/vulkan.hpp"

#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"

#define VK_CHECK(x)                                                                                                    \
    {                                                                                                                  \
        vk::Result err = x;                                                                                            \
        if (err != vk::Result::eSuccess)                                                                               \
        {                                                                                                              \
            spdlog::error("[vulkan]: {}", vk::to_string(err));                                                         \
            abort();                                                                                                   \
        }                                                                                                              \
    }\
