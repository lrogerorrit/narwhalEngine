#include "black_hole_compute_system.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <array>
#include <iostream>

namespace narwhal {
	BlackHoleComputeSystem::BlackHoleComputeSystem(NarwhalDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout setLayout): narwhalDevice{device}
	{
		createPipelineLayout(setLayout);
		createPipelines(renderPass);
	}
	BlackHoleComputeSystem::~BlackHoleComputeSystem()
	{
		vkDestroyPipelineLayout(narwhalDevice.device(), pipelineLayout, nullptr);
	}
	void BlackHoleComputeSystem::createPipelineLayout(VkDescriptorSetLayout setLayout)
	{
		std::vector<VkDescriptorSetLayout> descriptorSetLayout{ setLayout };
		
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout(narwhalDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void BlackHoleComputeSystem::createPipelines(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");
		
		PipelineConfigInfo pipelineConfig{};
		NarwhalPipeline::defaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		schwarzchildPipeline= std::make_unique<NarwhalPipeline>(narwhalDevice, "data/shaders/schwarzchildUpdate.comp.spv", pipelineConfig);
		kerrPipeline = std::make_unique<NarwhalPipeline>(narwhalDevice, "data/shaders/kerrUpdate.comp.spv", pipelineConfig);

	}

	void BlackHoleComputeSystem::render(FrameInfo& frameInfo,BlackHoleParameters& parameters)
	{
		vkResetFences(narwhalDevice.device(), 1, &frameInfo.computeFence);
		VkCommandBuffer commandBuffer = narwhalDevice.beginSingleTimeCommands();
		NarwhalPipeline& pipeline = parameters.blackHoleType == BlackHoleType::Schwarzchild ? *schwarzchildPipeline : *kerrPipeline;
		pipeline.bind(commandBuffer);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &frameInfo.computeDescriptorSet, 0, nullptr);
		vkCmdDispatch(commandBuffer, 1, 1, 1); //TODO: Calculate dispatch size

		narwhalDevice.endSingleTimeCommands(commandBuffer,frameInfo.computeFence);
		
	}
}
