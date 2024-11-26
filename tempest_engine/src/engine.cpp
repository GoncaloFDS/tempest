#include "engine.h"

#include "VkBootstrap.h"
#include "flecs.h"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#define GLFW_INCLUDE_VULKAN
#include <fstream>

#include "GLFW/glfw3.h"
#include "rendering/vulkan/vk_images.h"
#include "rendering/vulkan/vk_initializers.h"
#include "rendering/vulkan/vk_pipelines.h"
#include "slang/slang-com-ptr.h"
#include "slang/slang.h"
#include "spdlog/spdlog.h"

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
}

void Engine::Cleanup()
{
}

void Engine::Draw()
{
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
}

void Engine::InitSwapchain()
{
}

void Engine::InitCommands()
{
}

void Engine::InitSyncStructures()
{
}

void Engine::InitDescriptors()
{
}

void Engine::InitShaderCompiler()
{
}

void Engine::InitPipelines()
{
}

void Engine::InitBackgroundPipelines()
{
}

void Engine::CreateSwapchain(uint32_t width, uint32_t height)
{
}

void Engine::DestroySwapchain()
{
}

void Engine::DrawBackground(vk::CommandBuffer cmd)
{
}
