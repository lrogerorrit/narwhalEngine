#pragma once

#include "narwhal_device.hpp"

//std
#include <string>
#include <vector>

namespace narwhal {

	enum class PipelineType {
		GRAPHICS,
		COMPUTE,
		UNKNOWN
	};

	struct PipelineConfigInfo {
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo() = default;
		
		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		
		
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
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
		
		NarwhalPipeline(
			NarwhalDevice& device,
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const std::string& compFilepath,
			const PipelineConfigInfo& configInfo);

		NarwhalPipeline(
			NarwhalDevice& device,
			const std::string& compFilepath,
			const PipelineConfigInfo& configInfo);

		~NarwhalPipeline();

		void createPipelineCache();


		NarwhalPipeline(const NarwhalPipeline&) = delete;
		NarwhalPipeline& operator=(const NarwhalPipeline&) = delete;
		NarwhalPipeline() = default;

		void bind(VkCommandBuffer commandBuffer);
		
		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

		PipelineType getPipelineType() const { return pipelineType; }

		static void memoryBarrier(VkMemoryBarrier* memoryBarrier, VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags);
		
		
		
	private:
		void addShaderStage(VkPipelineShaderStageCreateInfo shaderStage[], int pos, VkShaderStageFlagBits stage, VkShaderModule shaderModule, std::string entryPoint, VkSpecializationInfo* specializationInfo= nullptr);

		VkGraphicsPipelineCreateInfo makePipelineCreateInfo(int stageCount, VkPipelineShaderStageCreateInfo shaderStages[], VkPipelineVertexInputStateCreateInfo& vertexInputInfo, const PipelineConfigInfo& configInfo);

		void createComputePipeline(const std::string& compFilepath, const PipelineConfigInfo& configInfo);
		
		void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);

		void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const std::string& compFilepath, const PipelineConfigInfo& configInfo);
		
		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		
		NarwhalDevice& narwhalDevice;
		
		VkPipeline pipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
		VkShaderModule compShaderModule;
		VkPipelineCache pipelineCache;
		
		PipelineType pipelineType = PipelineType::UNKNOWN;
	};
}

