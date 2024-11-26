#include "engine.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#include "vk_mem_alloc.hpp"

#include "VkBootstrap.h"
#include "flecs.h"

#include "GLFW/glfw3.h"
#include "rendering/vulkan/vk_images.h"
#include "rendering/vulkan/vk_initializers.h"
#include "rendering/vulkan/vk_pipelines.h"
#include "slang/slang-com-ptr.h"
#include "spdlog/spdlog.h"

#define GLFW_INCLUDE_VULKAN

constexpr bool USE_VALIDATION_LAYERS = true;

static Engine *LOADED_ENGINE = nullptr;

Engine &Engine::Get()
{
    return *LOADED_ENGINE;
}

void Engine::Init()
{
    assert(!LOADED_ENGINE);
    LOADED_ENGINE = this;

    InitWindow();

    InitVulkan();
    InitSwapchain();
    InitCommands();
    InitSyncStructures();
    InitDescriptors();
    InitShaderCompiler();
    InitPipelines();

    _isInitialized = true;
}

void Engine::Cleanup()
{
}

void Engine::Draw()
{
    auto &currentFrame = GetCurrentFrame();

    // wait until gpu has finished rendering last frame
    _device.waitForFences({currentFrame.renderFence}, true, 1000000000);
    currentFrame.deletionQueue.Flush();
    _device.resetFences({currentFrame.renderFence});

    uint32_t swapchainImageIndex;
    _device.acquireNextImageKHR(_swapchain, 1000000000, currentFrame.swapchainSemaphore, nullptr, &swapchainImageIndex);

    const vk::CommandBuffer cmd = currentFrame.mainCommandBuffer;
    cmd.reset();

    _drawExtent.width = _drawImage.imageExtent.width;
    _drawExtent.height = _drawImage.imageExtent.height;

    cmd.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    vkutils::TransitionImage(cmd, _drawImage.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);

    DrawBackground(cmd);

    vkutils::TransitionImage(cmd, _drawImage.image, vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal);
    vkutils::TransitionImage(cmd, _swapchainImages[swapchainImageIndex], vk::ImageLayout::eUndefined,
                             vk::ImageLayout::eTransferDstOptimal);

    vkutils::CopyImageToImage(cmd, _drawImage.image, _swapchainImages[swapchainImageIndex], _drawExtent,
                              _swapchainExtent);

    vkutils::TransitionImage(cmd, _swapchainImages[swapchainImageIndex], vk::ImageLayout::eTransferDstOptimal,
                             vk::ImageLayout::ePresentSrcKHR);

    cmd.end();

    vk::SemaphoreSubmitInfo WaitSemaphoreInfo = vkutils::SemaphoreSubmitInfo(
        vk::PipelineStageFlagBits2::eColorAttachmentOutput, currentFrame.swapchainSemaphore);
    vk::SemaphoreSubmitInfo SignalSemaphoreInfo =
        vkutils::SemaphoreSubmitInfo(vk::PipelineStageFlagBits2::eAllGraphics, currentFrame.renderSemaphore);

    vk::CommandBufferSubmitInfo cmdSubmitInfo{cmd};
    vk::SubmitInfo2 submit = vkutils::SubmitInfo(&cmdSubmitInfo, &SignalSemaphoreInfo, &WaitSemaphoreInfo);

    _graphicsQueue.submit2(submit, currentFrame.renderFence);

    // present
    vk::PresentInfoKHR presentInfo = {};
    presentInfo.setSwapchains(_swapchain);
    presentInfo.setWaitSemaphores(currentFrame.renderSemaphore);
    presentInfo.setImageIndices(swapchainImageIndex);
    _graphicsQueue.presentKHR(presentInfo);

    _frameNumber++;
}

void Engine::Run()
{
    while (!glfwWindowShouldClose(_window))
    {
        glfwSwapBuffers(_window);
        glfwPollEvents();

        Draw();
    }
}

void Engine::InitWindow()
{
    if (!glfwInit())
        return;

    if (!glfwVulkanSupported())
    {
        spdlog::critical("GLFW Vulkan not supported!");
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    _window = glfwCreateWindow(static_cast<int>(_windowExtent.width), static_cast<int>(_windowExtent.height),
                               "Tome Engine", nullptr, nullptr);
    if (!_window)
    {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(_window);
}

void Engine::InitVulkan()
{
    vkb::InstanceBuilder instanceBuilder;
    auto instanceResult = instanceBuilder.set_app_name("Tome App")
                              .request_validation_layers(USE_VALIDATION_LAYERS)
                              .use_default_debug_messenger()
                              .require_api_version(1, 3, 0)
                              .build();

    vkb::Instance vkbInstance = instanceResult.value();

    _instance = vkbInstance.instance;
    _debugMessenger = vkbInstance.debug_messenger;

    VkSurfaceKHR surface;
    glfwCreateWindowSurface(_instance, _window, nullptr, &surface);
    _surface = surface;

    vk::PhysicalDeviceVulkan13Features features13 = {};
    features13.dynamicRendering = true;
    features13.synchronization2 = true;

    vk::PhysicalDeviceVulkan12Features features12{};
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;

    vkb::PhysicalDeviceSelector physicalDeviceSelector{vkbInstance};
    vkb::PhysicalDevice physicalDevice = physicalDeviceSelector.set_minimum_version(1, 3)
                                             .set_required_features_13(features13)
                                             .set_required_features_12(features12)
                                             .set_surface(_surface)
                                             .select()
                                             .value();

    vkb::DeviceBuilder deviceBuilder{physicalDevice};
    vkb::Device vkbDevice = deviceBuilder.build().value();
    _device = vkbDevice.device;
    _chosenGpu = physicalDevice.physical_device;

    _graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    _graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    vma::AllocatorCreateInfo vmaAllocatorCreateInfo = {};
    vmaAllocatorCreateInfo.physicalDevice = _chosenGpu;
    vmaAllocatorCreateInfo.device = _device;
    vmaAllocatorCreateInfo.instance = _instance;
    vmaAllocatorCreateInfo.flags = vma::AllocatorCreateFlagBits::eBufferDeviceAddress;
    createAllocator(&vmaAllocatorCreateInfo, &_allocator);

    _deletionQueue.PushFunction([&]() {
        spdlog::info("Deleting allocator");
        _allocator.destroy();
    });
}

void Engine::InitSwapchain()
{
    CreateSwapchain(_windowExtent.width, _windowExtent.height);

    vk::Extent3D drawImageExtent = {_windowExtent.width, _windowExtent.height, 1};

    _drawImage.imageFormat = vk::Format::eR16G16B16A16Sfloat;
    _drawImage.imageExtent = drawImageExtent;

    vk::ImageUsageFlags drawImageUsages{vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
                                        vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eColorAttachment};

    vk::ImageCreateInfo renderImageInfo =
        vkutils::ImageCreateInfo(_drawImage.imageFormat, drawImageUsages, drawImageExtent);

    vma::AllocationCreateInfo renderImageAllocInfo = {};
    renderImageAllocInfo.usage = vma::MemoryUsage::eGpuOnly;
    renderImageAllocInfo.requiredFlags = {vk::MemoryPropertyFlagBits::eDeviceLocal};

    _allocator.createImage(&renderImageInfo, &renderImageAllocInfo, &_drawImage.image, &_drawImage.allocation,
                           &_drawImage.allocationInfo);

    vk::ImageViewCreateInfo renderImageViewCreateInfo =
        vkutils::ImageviewCreateInfo(_drawImage.imageFormat, _drawImage.image, vk::ImageAspectFlagBits::eColor);

    _device.createImageView(&renderImageViewCreateInfo, nullptr, &_drawImage.imageView);

    _deletionQueue.PushFunction([&]() {
        spdlog::info("Deleting image");
        _device.destroyImageView(_drawImage.imageView, nullptr);
        _allocator.destroyImage(_drawImage.image, _drawImage.allocation);
    });
}

void Engine::InitCommands()
{
    vk::CommandPoolCreateInfo commandPoolCreateInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                                    _graphicsQueueFamily};

    for (auto &frame : _frames)
    {
        _device.createCommandPool(&commandPoolCreateInfo, nullptr, &frame.commandPool);
        vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vkutils::CommandBufferAllocateInfo(frame.commandPool);
        _device.allocateCommandBuffers(&commandBufferAllocateInfo, &frame.mainCommandBuffer);
    }
}

void Engine::InitSyncStructures()
{
    vk::FenceCreateInfo fenceCreateInfo{vk::FenceCreateFlagBits::eSignaled};
    vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
    for (auto &frame : _frames)
    {
        _device.createFence(&fenceCreateInfo, nullptr, &frame.renderFence);
        _device.createSemaphore(&semaphoreCreateInfo, nullptr, &frame.swapchainSemaphore);
        _device.createSemaphore(&semaphoreCreateInfo, nullptr, &frame.renderSemaphore);
    }
}

void Engine::InitDescriptors()
{
    std::vector<DescriptorAllocator::PoolSizeRatio> poolSizeRatios = {
        {.descriptorType = vk::DescriptorType::eStorageImage, .ratio = 1}};

    _globalDescriptorAllocator.InitPool(_device, 10, poolSizeRatios);

    {
        DescriptorLayoutBuilder descriptorLayoutBuilder;
        descriptorLayoutBuilder.AddBinding(0, vk::DescriptorType::eStorageImage);
        _drawImageDescriptorSetLayout = descriptorLayoutBuilder.Build(_device, vk::ShaderStageFlagBits::eCompute);
    }

    _drawImageDescriptorSet = _globalDescriptorAllocator.Allocate(_device, _drawImageDescriptorSetLayout);

    vk::DescriptorImageInfo descriptorImageInfo = {};
    descriptorImageInfo.imageLayout = vk::ImageLayout::eGeneral;
    descriptorImageInfo.imageView = _drawImage.imageView;

    vk::WriteDescriptorSet drawImageWrite = {};
    drawImageWrite.dstBinding = 0;
    drawImageWrite.dstSet = _drawImageDescriptorSet;
    drawImageWrite.descriptorCount = 1;
    drawImageWrite.descriptorType = vk::DescriptorType::eStorageImage;
    drawImageWrite.pImageInfo = &descriptorImageInfo;

    _device.updateDescriptorSets({drawImageWrite}, {});

    _deletionQueue.PushFunction([&]() {
        _globalDescriptorAllocator.DestroyPool(_device);
        _device.destroyDescriptorSetLayout(_drawImageDescriptorSetLayout, nullptr);
    });
}

void Engine::InitShaderCompiler()
{
    using namespace slang;

    createGlobalSession(_slangGlobalSession.writeRef());

    SessionDesc sessionDesc = {};

    TargetDesc targetDesc = {};
    targetDesc.format = SLANG_SPIRV;
    targetDesc.profile = _slangGlobalSession->findProfile("spirv_1_5");
    targetDesc.flags = 0;

    std::vector searchPaths = {"shaders/", "../tome_engine/shaders/"};
    sessionDesc.searchPaths = searchPaths.data();
    sessionDesc.searchPathCount = static_cast<uint32_t>(searchPaths.size());

    sessionDesc.targets = &targetDesc;
    sessionDesc.targetCount = 1;

    std::vector<CompilerOptionEntry> compilerOptions;
    compilerOptions.push_back({.name = CompilerOptionName::EmitSpirvDirectly,
                               .value = {.kind = CompilerOptionValueKind::Int,
                                         .intValue0 = 1,
                                         .intValue1 = 0,
                                         .stringValue0 = nullptr,
                                         .stringValue1 = nullptr}});
    sessionDesc.compilerOptionEntries = compilerOptions.data();
    sessionDesc.compilerOptionEntryCount = static_cast<uint32_t>(compilerOptions.size());

    _slangGlobalSession->createSession(sessionDesc, _slangSession.writeRef());
}

void Engine::InitPipelines()
{
    InitBackgroundPipelines();
}

void Engine::InitBackgroundPipelines()
{
    vk::PipelineLayoutCreateInfo computeLayout{};
    computeLayout.pSetLayouts = &_drawImageDescriptorSetLayout;
    computeLayout.setLayoutCount = 1;

    _device.createPipelineLayout(&computeLayout, nullptr, &_gradientPipelineLayout);

    auto computeDrawShader = vkutils::LoadShaderModule("gradient.slang", _device, _slangSession);
    if (!computeDrawShader.has_value())
    {
        return;
    }

    vk::PipelineShaderStageCreateInfo stageCreateInfo = {};
    stageCreateInfo.stage = vk::ShaderStageFlagBits::eCompute;
    stageCreateInfo.module = computeDrawShader.value();
    stageCreateInfo.pName = "main";

    vk::ComputePipelineCreateInfo computePipelineCreateInfo{};
    computePipelineCreateInfo.layout = _gradientPipelineLayout;
    computePipelineCreateInfo.stage = stageCreateInfo;

    _gradientPipeline = _device.createComputePipeline({}, computePipelineCreateInfo, nullptr).value;
    _device.destroyShaderModule(computeDrawShader.value(), nullptr);

    _deletionQueue.PushFunction([&]() {
        _device.destroyPipelineLayout(_gradientPipelineLayout, nullptr);
        _device.destroyPipeline(_gradientPipeline, nullptr);
    });
}

void Engine::CreateSwapchain(uint32_t width, uint32_t height)
{
    vkb::SwapchainBuilder swapchainBuilder{_chosenGpu, _device, _surface};
    _swapchainImageFormat = vk::Format::eB8G8R8A8Unorm;

    vk::SurfaceFormatKHR SurfaceFormat{_swapchainImageFormat, vk::ColorSpaceKHR::eSrgbNonlinear};
    vkb::Swapchain vkbSwapchain =
        swapchainBuilder.set_desired_format(SurfaceFormat)
            .set_desired_present_mode(static_cast<VkPresentModeKHR>(vk::PresentModeKHR::eFifo))
            .set_desired_extent(width, height)
            .add_image_usage_flags(static_cast<VkImageUsageFlags>(vk::ImageUsageFlagBits::eTransferDst))
            .build()
            .value();

    _swapchainExtent = vkbSwapchain.extent;
    _swapchain = vkbSwapchain.swapchain;
    for (VkImage_T *image : vkbSwapchain.get_images().value())
    {
        _swapchainImages.push_back(image);
    }
    for (auto imageView : vkbSwapchain.get_image_views().value())
    {
        _swapchainImageViews.push_back(imageView);
    }
}

void Engine::DestroySwapchain()
{
    _device.destroySwapchainKHR(_swapchain, nullptr);
    for (auto imageView : _swapchainImageViews)
    {
        _device.destroyImageView(imageView, nullptr);
    }
}

void Engine::DrawBackground(vk::CommandBuffer cmd)
{
    cmd.bindPipeline(vk::PipelineBindPoint::eCompute, _gradientPipeline);
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute, _gradientPipelineLayout, 0, _drawImageDescriptorSet,
                           nullptr);
    cmd.dispatch(std::ceil(_drawExtent.width / 16.0), std::ceil(_drawExtent.height / 16.0), 1);
}
