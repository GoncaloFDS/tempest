#pragma once 
#include <slang/slang-com-ptr.h>

#include "vk_types.h"

namespace vkutils {
 std::optional<VkShaderModule> LoadShaderModule(const char* shaderFileName, vk::Device device, Slang::ComPtr<slang::ISession> slangSession);


};