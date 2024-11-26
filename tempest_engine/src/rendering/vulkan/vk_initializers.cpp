#include "vk_initializers.h"

vk::CommandPoolCreateInfo vkutils::CommanPollCreateInfo(uint32_t queueFamilyIndex,
                                                        vk::CommandPoolCreateFlags flags /*= 0*/)
{
    vk::CommandPoolCreateInfo info = {};
    info.queueFamilyIndex = queueFamilyIndex;
    info.flags = flags;
    return info;
}

vk::CommandBufferAllocateInfo vkutils::CommandBufferAllocateInfo(vk::CommandPool pool, uint32_t count /*= 1*/)
{
    vk::CommandBufferAllocateInfo info = {};
    info.commandPool = pool;
    info.commandBufferCount = count;
    info.level = vk::CommandBufferLevel::ePrimary;
    return info;
}

vk::CommandBufferBeginInfo vkutils::CommandBufferBeginInfo(vk::CommandBufferUsageFlags flags /*= 0*/)
{
    vk::CommandBufferBeginInfo info = {};
    info.pInheritanceInfo = nullptr;
    info.flags = flags;
    return info;
}

vk::FenceCreateInfo vkutils::FenceCreateInfo(vk::FenceCreateFlags flags /*= 0*/)
{
    vk::FenceCreateInfo info = {};
    info.flags = flags;
    return info;
}

vk::SemaphoreCreateInfo vkutils::SemaphoreCreateInfo(vk::SemaphoreCreateFlags flags /*= 0*/)
{
    vk::SemaphoreCreateInfo info = {};
    info.flags = flags;
    return info;
}

vk::SemaphoreSubmitInfo vkutils::SemaphoreSubmitInfo(vk::PipelineStageFlags2 stageMask, vk::Semaphore semaphore)
{
    vk::SemaphoreSubmitInfo submitInfo{};
    submitInfo.semaphore = semaphore;
    submitInfo.stageMask = stageMask;
    submitInfo.deviceIndex = 0;
    submitInfo.value = 1;

    return submitInfo;
}

vk::CommandBufferSubmitInfo vkutils::CommandBufferSubmitInfo(vk::CommandBuffer cmd)
{
    vk::CommandBufferSubmitInfo info{};
    info.commandBuffer = cmd;
    info.deviceMask = 0;

    return info;
}

vk::SubmitInfo2 vkutils::SubmitInfo(vk::CommandBufferSubmitInfo *cmd, vk::SemaphoreSubmitInfo *signalSemaphoreInfo,
                                    vk::SemaphoreSubmitInfo *waitSemaphoreInfo)
{
    vk::SubmitInfo2 info = {};

    info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
    info.pWaitSemaphoreInfos = waitSemaphoreInfo;

    info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
    info.pSignalSemaphoreInfos = signalSemaphoreInfo;

    info.commandBufferInfoCount = 1;
    info.pCommandBufferInfos = cmd;

    return info;
}

vk::PresentInfoKHR vkutils::PresentInfo()
{
    vk::PresentInfoKHR info = {};
    info.swapchainCount = 0;
    info.pSwapchains = nullptr;
    info.pWaitSemaphores = nullptr;
    info.waitSemaphoreCount = 0;
    info.pImageIndices = nullptr;

    return info;
}

vk::RenderingAttachmentInfo vkutils::AttachmentInfo(
    vk::ImageView view, vk::ClearValue *clear, vk::ImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/)
{
    vk::RenderingAttachmentInfo colorAttachment{};

    colorAttachment.imageView = view;
    colorAttachment.imageLayout = layout;
    colorAttachment.loadOp = clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    if (clear)
    {
        colorAttachment.clearValue = *clear;
    }

    return colorAttachment;
}

vk::RenderingAttachmentInfo vkutils::DepthAttachmentInfo(
    vk::ImageView view, vk::ImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/)
{
    vk::RenderingAttachmentInfo depthAttachment{};
    depthAttachment.imageView = view;
    depthAttachment.imageLayout = layout;
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    depthAttachment.clearValue.depthStencil.depth = 0.f;

    return depthAttachment;
}

vk::RenderingInfo vkutils::RenderingInfo(vk::Extent2D renderExtent, vk::RenderingAttachmentInfo *colorAttachment,
                                         vk::RenderingAttachmentInfo *depthAttachment)
{
    vk::RenderingInfo renderInfo{};

    renderInfo.renderArea = vk::Rect2D{vk::Offset2D{0, 0}, renderExtent};
    renderInfo.layerCount = 1;
    renderInfo.colorAttachmentCount = 1;
    renderInfo.pColorAttachments = colorAttachment;
    renderInfo.pDepthAttachment = depthAttachment;
    renderInfo.pStencilAttachment = nullptr;

    return renderInfo;
}

vk::ImageSubresourceRange vkutils::ImageSubresourceRange(vk::ImageAspectFlags aspectMask)
{
    vk::ImageSubresourceRange subImage = {};
    subImage.aspectMask = aspectMask;
    subImage.baseMipLevel = 0;
    subImage.levelCount = VK_REMAINING_MIP_LEVELS;
    subImage.baseArrayLayer = 0;
    subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

    return subImage;
}

vk::DescriptorSetLayoutBinding vkutils::DescriptorsetLayoutBinding(vk::DescriptorType type,
                                                                   vk::ShaderStageFlags stageFlags, uint32_t binding)
{
    vk::DescriptorSetLayoutBinding setbind = {};
    setbind.binding = binding;
    setbind.descriptorCount = 1;
    setbind.descriptorType = type;
    setbind.pImmutableSamplers = nullptr;
    setbind.stageFlags = stageFlags;

    return setbind;
}

vk::DescriptorSetLayoutCreateInfo vkutils::DescriptorsetLayoutCreateInfo(vk::DescriptorSetLayoutBinding *bindings,
                                                                         uint32_t bindingCount)
{
    vk::DescriptorSetLayoutCreateInfo info = {};
    info.pBindings = bindings;
    info.bindingCount = bindingCount;
    info.flags = {};

    return info;
}

vk::WriteDescriptorSet vkutils::WriteDescriptorImage(vk::DescriptorType type, vk::DescriptorSet dstSet,
                                                     vk::DescriptorImageInfo *imageInfo, uint32_t binding)
{
    vk::WriteDescriptorSet write = {};
    write.dstBinding = binding;
    write.dstSet = dstSet;
    write.descriptorCount = 1;
    write.descriptorType = type;
    write.pImageInfo = imageInfo;
    return write;
}

vk::WriteDescriptorSet vkutils::WriteDescriptorBuffer(vk::DescriptorType type, vk::DescriptorSet dstSet,
                                                      vk::DescriptorBufferInfo *bufferInfo, uint32_t binding)
{
    vk::WriteDescriptorSet write = {};
    write.dstBinding = binding;
    write.dstSet = dstSet;
    write.descriptorCount = 1;
    write.descriptorType = type;
    write.pBufferInfo = bufferInfo;
    return write;
}

vk::DescriptorBufferInfo vkutils::BufferInfo(vk::Buffer buffer, vk::DeviceSize offset, vk::DeviceSize range)
{
    vk::DescriptorBufferInfo binfo{};
    binfo.buffer = buffer;
    binfo.offset = offset;
    binfo.range = range;
    return binfo;
}

vk::ImageCreateInfo vkutils::ImageCreateInfo(vk::Format format, vk::ImageUsageFlags usageFlags, vk::Extent3D extent)
{
    vk::ImageCreateInfo info = {};
    info.imageType = vk::ImageType::e2D;

    info.format = format;
    info.extent = extent;

    info.mipLevels = 1;
    info.arrayLayers = 1;

    // for MSAA. we will not be using it by default, so default it to 1 sample per pixel.
    info.samples = vk::SampleCountFlagBits::e1;

    // optimal tiling, which means the image is stored on the best gpu format
    info.tiling = vk::ImageTiling::eOptimal;
    info.usage = usageFlags;

    return info;
}

vk::ImageViewCreateInfo vkutils::ImageviewCreateInfo(vk::Format format, vk::Image image,
                                                     vk::ImageAspectFlags aspectFlags)
{
    // build a image-view for the depth image to use for rendering
    vk::ImageViewCreateInfo info = {};

    info.viewType = vk::ImageViewType::e2D;
    info.image = image;
    info.format = format;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    info.subresourceRange.aspectMask = aspectFlags;

    return info;
}

vk::PipelineLayoutCreateInfo vkutils::PipelineLayoutCreateInfo()
{
    vk::PipelineLayoutCreateInfo info{};
    // empty defaults
    info.flags = {};
    info.setLayoutCount = 0;
    info.pSetLayouts = nullptr;
    info.pushConstantRangeCount = 0;
    info.pPushConstantRanges = nullptr;
    return info;
}

vk::PipelineShaderStageCreateInfo vkutils::PipelineShaderStageCreateInfo(vk::ShaderStageFlagBits stage,
                                                                         vk::ShaderModule shaderModule,
                                                                         const char *entry)
{
    vk::PipelineShaderStageCreateInfo info{};
    // shader stage
    info.stage = stage;
    // module containing the code for this shader stage
    info.module = shaderModule;
    // the entry point of the shader
    info.pName = entry;
    return info;
}