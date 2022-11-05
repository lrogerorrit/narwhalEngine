#pragma once

#include "narwhal_device.hpp"

//std
#include <string>
#include <vector>

namespace narwhal {

	struct PipelineConfigInfo {
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};
	
	class NarwhalPipeline
	{
	public:
		NarwhalPipeline(
			NarwhalDevice& device,
			const std::string& vertFilepath,
			const std::string& fragFilepath, 
			const PipelineConfigInfo& configInfo);

		~NarwhalPipeline();

		NarwhalPipeline(const NarwhalPipeline&) = delete;
		NarwhalPipeline& operator=(const NarwhalPipeline&) = delete;
		
		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
		
	private:
		
		void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
		
		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		
		NarwhalDevice& narwhalDevice;
		
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}

