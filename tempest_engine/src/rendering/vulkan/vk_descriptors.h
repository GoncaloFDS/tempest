#pragma once

struct DescriptorLayoutBuilder
{
    std::vector<vk::DescriptorSetLayoutBinding> bindings;

    DescriptorLayoutBuilder &AddBinding(uint32_t binding, vk::DescriptorType type);
    void Clear();

    vk::DescriptorSetLayout Build(vk::Device device, vk::ShaderStageFlags shaderStages,
                                  vk::DescriptorSetLayoutCreateFlags flags = {}, void *pNext = nullptr);
};

struct DescriptorAllocator
{
    struct PoolSizeRatio
    {
        vk::DescriptorType descriptorType;
        float ratio;
    };

    vk::DescriptorPool descriptorPool;

    void InitPool(vk::Device device, uint32_t maxSets, std::span<PoolSizeRatio> poolSizeRatios);
    void ClearDescriptors(vk::Device device) const;
    void DestroyPool(vk::Device device) const;

    vk::DescriptorSet Allocate(vk::Device device, vk::DescriptorSetLayout descriptorSetLayout) const;
};