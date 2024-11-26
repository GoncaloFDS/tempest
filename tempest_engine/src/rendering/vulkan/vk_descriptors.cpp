#include "vk_descriptors.h"

DescriptorLayoutBuilder &DescriptorLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type) {
    VkDescriptorSetLayoutBinding newBind = {};
    newBind.binding = binding;
    newBind.descriptorCount = 1;
    newBind.descriptorType = type;

    bindings.push_back(newBind);
    return *this;
}

void DescriptorLayoutBuilder::Clear() {
    bindings.clear();
}

VkDescriptorSetLayout DescriptorLayoutBuilder::Build(
    VkDevice device,
    VkShaderStageFlags shaderStages,
    VkDescriptorSetLayoutCreateFlags flags,
    void *pNext) {
    for (auto &binding : bindings) {
        binding.stageFlags |= shaderStages;
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext = pNext;
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorSetLayoutCreateInfo.flags = flags;

    VkDescriptorSetLayout descriptorSetLayout;
    VK_CHECK(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));

    return descriptorSetLayout;
}

void DescriptorAllocator::InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolSizeRatios) {
    std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
    for (auto [descriptorType, ratio] : poolSizeRatios) {
        descriptorPoolSizes.push_back(
            VkDescriptorPoolSize{
                .type = descriptorType,
                .descriptorCount = static_cast<uint32_t>(ratio) * maxSets });
    }
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.flags = 0;
    descriptorPoolCreateInfo.maxSets = maxSets;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizeRatios.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

    vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
}

void DescriptorAllocator::ClearDescriptors(VkDevice device) const {
    vkResetDescriptorPool(device, descriptorPool, 0);
}

void DescriptorAllocator::DestroyPool(VkDevice device) const {
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

VkDescriptorSet DescriptorAllocator::Allocate(VkDevice device, VkDescriptorSetLayout descriptorSetLayout) const {
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

    VkDescriptorSet descriptorSet;
    VK_CHECK(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet));

    return descriptorSet;
}