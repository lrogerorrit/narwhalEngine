#include "quad_render_system.hpp"


//std
#include <stdexcept>
#include <array>
#include <iostream>


namespace narwhal {
	QuadRenderSystem::QuadRenderSystem(NarwhalDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout setLayout) : narwhalDevice{ device } {
		createPipelineLayout(setLayout);
		createPipelines(renderPass);
		quadModel = NarwhalModel::createModelFromFile(narwhalDevice, "data/models/quad.obj");
	}

	QuadRenderSystem::~QuadRenderSystem()
	{
		vkDestroyPipelineLayout(narwhalDevice.device(), pipelineLayout, nullptr);
	}
	void QuadRenderSystem::createPipelineLayout(VkDescriptorSetLayout setLayout)
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
	void QuadRenderSystem::createPipelines(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		NarwhalPipeline::defaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipeline = std::make_unique<NarwhalPipeline>(narwhalDevice, "data/shaders/quad.vert.spv", "data/shaders/quad.frag.spv", pipelineConfig);
	}

	void QuadRenderSystem::render(QuadFrameInfo& frameInfo)
	{
		pipeline->bind(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS);

		vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.renderDescriptorSet, 0, nullptr);
		quadModel->bind(frameInfo.commandBuffer);
		quadModel->draw(frameInfo.commandBuffer);
	}
}