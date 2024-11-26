#include "vk_descriptors.h"

#include "vk_types.h"

DescriptorLayoutBuilder &DescriptorLayoutBuilder::AddBinding(uint32_t binding, vk::DescriptorType type)
{
    vk::DescriptorSetLayoutBinding newBind = {};
    newBind.binding = binding;
    newBind.descriptorCount = 1;
    newBind.descriptorType = type;

    bindings.push_back(newBind);
    return *this;
}

void DescriptorLayoutBuilder::Clear()
{
    bindings.clear();
}

vk::DescriptorSetLayout DescriptorLayoutBuilder::Build(vk::Device device, vk::ShaderStageFlags shaderStages,
                                                       vk::DescriptorSetLayoutCreateFlags flags, void *pNext)
{
    for (auto &binding : bindings)
    {
        binding.stageFlags |= shaderStages;
    }

    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.pNext = pNext;
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorSetLayoutCreateInfo.flags = flags;

    vk::DescriptorSetLayout descriptorSetLayout;
    auto R = device.createDescriptorSetLayout(&descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);

    return descriptorSetLayout;
}

void DescriptorAllocator::InitPool(vk::Device device, uint32_t maxSets, std::span<PoolSizeRatio> poolSizeRatios)
{
    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;
    for (auto [descriptorType, ratio] : poolSizeRatios)
    {
        descriptorPoolSizes.push_back({descriptorType, static_cast<uint32_t>(ratio) * maxSets});
    }
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.flags = {};
    descriptorPoolCreateInfo.maxSets = maxSets;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizeRatios.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

    auto R = device.createDescriptorPool(&descriptorPoolCreateInfo, nullptr, &descriptorPool);
}

void DescriptorAllocator::ClearDescriptors(vk::Device device) const
{
    vkResetDescriptorPool(device, descriptorPool, 0);
}

void DescriptorAllocator::DestroyPool(vk::Device device) const
{
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

vk::DescriptorSet DescriptorAllocator::Allocate(vk::Device device, vk::DescriptorSetLayout descriptorSetLayout) const
{
    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

    vk::DescriptorSet descriptorSet;
    auto R = device.allocateDescriptorSets(&descriptorSetAllocateInfo, &descriptorSet);

    return descriptorSet;
}
