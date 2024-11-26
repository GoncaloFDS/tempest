#pragma once

#include "vk_types.h"

struct DescriptorLayoutBuilder {
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    DescriptorLayoutBuilder &AddBinding(uint32_t binding, VkDescriptorType type);
    void Clear();

    VkDescriptorSetLayout Build(VkDevice device,
        VkShaderStageFlags shaderStages,
        VkDescriptorSetLayoutCreateFlags flags = 0,
        void *pNext = nullptr);
};

struct DescriptorAllocator {

    struct PoolSizeRatio {
        VkDescriptorType descriptorType;
        float ratio;
    };

    VkDescriptorPool descriptorPool;

    void InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolSizeRatios);
    void ClearDescriptors(VkDevice device) const;
    void DestroyPool(VkDevice device) const;

    VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout descriptorSetLayout) const;
};