#include "vk_pipelines.h"

std::optional<VkShaderModule> vkutils::LoadShaderModule(
    const char *shaderFileName,
    vk::Device device,
    Slang::ComPtr<slang::ISession> slangSession) {

    slang::IModule *slangModule;
    {
        Slang::ComPtr<slang::IBlob> sourceBlob;
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        slangModule = slangSession->loadModule(shaderFileName, diagnosticsBlob.writeRef());
        if (diagnosticsBlob) {
            spdlog::error("shader diagnostic: {}", static_cast<const char *>(diagnosticsBlob->getBufferPointer()));
        }
        if (!slangModule) {
            spdlog::error("shader failed to load");
            return {};
        }
    }

    Slang::ComPtr<slang::IEntryPoint> entryPoint;
    slangModule->findEntryPointByName("computeMain", entryPoint.writeRef());


    std::vector<slang::IComponentType *> componentTypes = { slangModule, entryPoint };


    Slang::ComPtr<slang::IComponentType> composedProgram;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        SlangResult result = slangSession->createCompositeComponentType(
            componentTypes.data(),
            componentTypes.size(),
            composedProgram.writeRef(),
            diagnosticsBlob.writeRef());
        if (diagnosticsBlob) {
            spdlog::error("shader diagnostic: {}", static_cast<const char *>(diagnosticsBlob->getBufferPointer()));
            return {};
        }
    }
    Slang::ComPtr<slang::IBlob> spirvCode;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        SlangResult result = composedProgram->getEntryPointCode(0, 0, spirvCode.writeRef(), diagnosticsBlob.writeRef());
        if (diagnosticsBlob) {
            spdlog::error("shader diagnostic: {}", static_cast<const char *>(diagnosticsBlob->getBufferPointer()));
            return {};
        }
    }

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.codeSize = spirvCode->getBufferSize();
    shaderModuleCreateInfo.pCode = static_cast<const uint32_t *>(spirvCode->getBufferPointer());

    vk::ShaderModule shaderModule;
    vk::Result R = device.createShaderModule(&shaderModuleCreateInfo, nullptr, &shaderModule);

    return { shaderModule };
}