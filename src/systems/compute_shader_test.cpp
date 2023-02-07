#include "compute_shader_test.hpp"

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


	ComputeTestSystem::ComputeTestSystem(NarwhalDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout ): narwhalDevice{device}
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	ComputeTestSystem::~ComputeTestSystem()
	{
		vkDestroyPipelineLayout(narwhalDevice.device(), pipelineLayout, nullptr);
	}

	void ComputeTestSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		//Same as  VkPushConstantRange pushConstantRange {VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData)};
		/*VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(ComputeTestPushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };
		

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;*/
		
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };
		
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		

		

		if (vkCreatePipelineLayout(narwhalDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	void ComputeTestSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		NarwhalPipeline::defaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		narwhalPipeline = std::make_unique<NarwhalPipeline>(narwhalDevice, "data/shaders/test_shader.comp.spv", pipelineConfig);

	}


	void ComputeTestSystem::renderGameObjects(FrameInfo& frameInfo){
		narwhalPipeline->bind(frameInfo.commandBuffer);


		/*vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);
		for (auto& kv : frameInfo.gameObjects) {
			auto& obj = kv.second;
			if (obj.model == nullptr) continue;

			ComputeTestPushConstantData push{};
			push.modelMatrix = obj.transform.mat4();
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ComputeTestPushConstantData), &push);
			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
		}*/
	}

		//TODO: https://arm-software.github.io/vulkan-sdk/basic_compute.html
	void ComputeTestSystem::render(FrameInfo& frameInfo)
	{
		// First, we have to wait until previous vertex shader invocations have completed
		// since we will overwrite the vertex buffer used in previous frame here.
		//
		// We only need execution barriers here and no memory barrier since we have a write-after-read hazard.
		// Write-after-read only requires execution barriers.
		// We have not touched the memory written by compute earlier, so no memory synchronization is needed.
		
		memoryBarrier(frameInfo.commandBuffer, 0, 0, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
		
		// Bind the compute pipeline.
		narwhalPipeline->bind(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE);
		
		// Bind descriptor set.
		vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &frameInfo.computeDescriptorSet, 0, nullptr);
		
		// Dispatch the compute job.
		// The number of workgroups is determined by the number of vertices in the vertex buffer.
		// We have 3 vertices in the vertex buffer, so we dispatch 3 workgroups.
		// Each workgroup will execute the compute shader once.
		vkCmdDispatch(frameInfo.commandBuffer, 1, 1, 1);
		
		// We have to wait until compute shader has finished writing to the vertex buffer.
		// We need a memory barrier here to ensure that the vertex shader reads from the vertex buffer after the compute shader has finished writing to the vertex buffer.
		// We have a write-after-write hazard here, so we need a memory barrier.
		// We have not touched the memory read by the vertex shader earlier, so no execution synchronization is needed.
		memoryBarrier(frameInfo.commandBuffer, 0, 0, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);
		
		
		

	}


}