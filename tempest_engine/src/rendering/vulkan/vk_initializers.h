#pragma once

#include "vk_types.h"

namespace vkutils
{

vk::CommandPoolCreateInfo CommanPollCreateInfo(uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags flags = {});

vk::CommandBufferAllocateInfo CommandBufferAllocateInfo(vk::CommandPool pool, uint32_t count = 1);

vk::CommandBufferBeginInfo CommandBufferBeginInfo(vk::CommandBufferUsageFlags flags = {});

vk::CommandBufferSubmitInfo CommandBufferSubmitInfo(vk::CommandBuffer cmd);

vk::FenceCreateInfo FenceCreateInfo(vk::FenceCreateFlags flags = {});

vk::SemaphoreCreateInfo SemaphoreCreateInfo(vk::SemaphoreCreateFlags flags = {});

vk::SubmitInfo2 SubmitInfo(vk::CommandBufferSubmitInfo *cmd, vk::SemaphoreSubmitInfo *signalSemaphoreInfo,
                           vk::SemaphoreSubmitInfo *waitSemaphoreInfo);

vk::PresentInfoKHR PresentInfo();

vk::RenderingAttachmentInfo AttachmentInfo(vk::ImageView view, vk::ClearValue *clear,
                                           vk::ImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);

vk::RenderingAttachmentInfo DepthAttachmentInfo(vk::ImageView view,
                                                vk::ImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);

vk::RenderingInfo RenderingInfo(vk::Extent2D renderExtent, vk::RenderingAttachmentInfo *colorAttachment,
                                vk::RenderingAttachmentInfo *depthAttachment);

vk::ImageSubresourceRange ImageSubresourceRange(vk::ImageAspectFlags aspectMask);

vk::SemaphoreSubmitInfo SemaphoreSubmitInfo(vk::PipelineStageFlags2 stageMask, vk::Semaphore semaphore);

vk::DescriptorSetLayoutBinding DescriptorsetLayoutBinding(vk::DescriptorType type, vk::ShaderStageFlags stageFlags,
                                                          uint32_t binding);

vk::DescriptorSetLayoutCreateInfo DescriptorsetLayoutCreateInfo(vk::DescriptorSetLayoutBinding *bindings,
                                                                uint32_t bindingCount);

vk::WriteDescriptorSet WriteDescriptorImage(vk::DescriptorType type, vk::DescriptorSet dstSet,
                                            vk::DescriptorImageInfo *imageInfo, uint32_t binding);

vk::WriteDescriptorSet WriteDescriptorBuffer(vk::DescriptorType type, vk::DescriptorSet dstSet,
                                             vk::DescriptorBufferInfo *bufferInfo, uint32_t binding);

vk::DescriptorBufferInfo BufferInfo(vk::Buffer buffer, vk::DeviceSize offset, vk::DeviceSize range);

vk::ImageCreateInfo ImageCreateInfo(vk::Format format, vk::ImageUsageFlags usageFlags, vk::Extent3D extent);
vk::ImageViewCreateInfo ImageviewCreateInfo(vk::Format format, vk::Image image, vk::ImageAspectFlags aspectFlags);
vk::PipelineLayoutCreateInfo PipelineLayoutCreateInfo();
vk::PipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(vk::ShaderStageFlagBits stage,
                                                                vk::ShaderModule shaderModule,
                                                                const char *entry = "main");
} // namespace vkutils