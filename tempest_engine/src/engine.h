#pragma once

#include "rendering/vulkan/vk_descriptors.h"
#include "rendering/vulkan/vk_types.h"
#include "slang/slang-com-ptr.h"
#include "slang/slang.h"

#include "vk_mem_alloc.hpp"

namespace slang
{
struct IGlobalSession;
}

struct AllocatedImage
{
    vk::Image image;
    vk::ImageView imageView;
    vma::Allocation allocation;
    vma::AllocationInfo allocationInfo;
    vk::Extent3D imageExtent;
    vk::Format imageFormat;
};

struct DeletionQueue
{
    std::deque<std::function<void()>> deletors;

    void PushFunction(std::function<void()> &&function)
    {
        deletors.push_back(function);
    }

    void Flush()
    {
        for (auto &deletor : std::ranges::reverse_view(deletors))
        {
            deletor();
        }
        deletors.clear();
    }
};

struct FrameData
{
    vk::CommandPool commandPool;
    vk::CommandBuffer mainCommandBuffer;
    vk::Semaphore swapchainSemaphore;
    vk::Semaphore renderSemaphore;
    vk::Fence renderFence;
    DeletionQueue deletionQueue;
};

constexpr unsigned int FRAME_OVERLAP = 2;

class Engine
{
  public:
    static Engine &Get();

    void Init();

    void Cleanup();

    void Draw();

    void Run();

    void ImmediateSubmit(std::function<void(vk::CommandBuffer cmd)> &&function);

  private:
    bool _isInitialized = false;
    int _frameNumber = 0;
    bool _shouldStopRendering = false;
    vk::Extent2D _windowExtent = {1700, 900};

    struct GLFWwindow *_window = nullptr;

    vk::Instance _instance = nullptr;
    vk::DebugUtilsMessengerEXT _debugMessenger = nullptr;
    vk::PhysicalDevice _chosenGpu = nullptr;
    vk::Device _device = nullptr;
    vk::SurfaceKHR _surface = nullptr;

    vk::SwapchainKHR _swapchain = nullptr;
    vk::Format _swapchainImageFormat = vk::Format::eR8G8B8A8Srgb;

    std::vector<vk::Image> _swapchainImages;
    std::vector<vk::ImageView> _swapchainImageViews;
    vk::Extent2D _swapchainExtent = {800, 600};

    FrameData _frames[FRAME_OVERLAP] = {};
    vk::Queue _graphicsQueue = nullptr;
    uint32_t _graphicsQueueFamily = 0;

    DeletionQueue _deletionQueue;

    vma::Allocator _allocator = nullptr;

    AllocatedImage _drawImage = {};
    vk::Extent2D _drawExtent = {};

    DescriptorAllocator _globalDescriptorAllocator = {};
    vk::DescriptorSet _drawImageDescriptorSet = nullptr;
    vk::DescriptorSetLayout _drawImageDescriptorSetLayout = nullptr;

    vk::Pipeline _gradientPipeline = nullptr;
    vk::PipelineLayout _gradientPipelineLayout = nullptr;

    Slang::ComPtr<slang::IGlobalSession> _slangGlobalSession;
    Slang::ComPtr<slang::ISession> _slangSession;

    // immediat submit structures
    vk::Fence _immFence;
    vk::CommandBuffer _immCommandBuffer;
    vk::CommandPool _immCommandPool;

    void InitWindow();
    void InitVulkan();
    void InitSwapchain();
    void InitCommands();
    void InitSyncStructures();
    void InitDescriptors();
    void InitShaderCompiler();
    void InitPipelines();
    void InitBackgroundPipelines();
    void InitImgui();

    void CreateSwapchain(uint32_t width, uint32_t height);
    void DestroySwapchain();

    FrameData &GetCurrentFrame();

    void DrawBackground(vk::CommandBuffer cmd);
    void DrawImgui(vk::CommandBuffer cmd, vk::ImageView targetImageView);
};
