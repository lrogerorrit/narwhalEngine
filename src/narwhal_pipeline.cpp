#include "narwhal_pipeline.hpp"
#include "utils/utils.hpp"

//std

#include <stdexcept>
#include <iostream>

namespace narwhal {
	
		NarwhalPipeline::NarwhalPipeline(NarwhalDevice& device,
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo) : narwhalDevice(device)
		{
			createGraphicsPipeline(vertFilepath, fragFilepath,configInfo);
		}


		PipelineConfigInfo NarwhalPipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height)
		{
			PipelineConfigInfo configInfo{};
			
			return configInfo;
		}

		void NarwhalPipeline::createGraphicsPipeline(const std::string & vertFilepath, const std::string & fragFilepath, const PipelineConfigInfo& configInfo)
		{
			// Read in shader files
			std::string vertCode, fragCode;
			if (!readFile(vertFilepath, vertCode) || !readFile(fragFilepath, fragCode))
			{
				throw std::runtime_error("Failed to create graphics pipeline!");
			}


			std::cout << "Vertex shader code size: " << vertCode.size() << std::endl;
			std::cout << "Fragment shader code size: " << fragCode.size() << std::endl;
		}

		void NarwhalPipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
		{
			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

			if (vkCreateShaderModule(narwhalDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create shader module!");
			}
		}
	
}