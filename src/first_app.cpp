#include "first_app.hpp"

//std
#include <stdexcept>
#include <array>

namespace narwhal {

	FirstApp::FirstApp()
	{
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	FirstApp::~FirstApp()
	{
		vkDestroyPipelineLayout(narwhalDevice.device(), pipelineLayout, nullptr);
	}
	
	void FirstApp::run()
	{
		while (!narwhalWindow.shouldClose())
		{
			glfwPollEvents();
			drawFrame();
		}
		
		vkDeviceWaitIdle(narwhalDevice.device());
	}
	void FirstApp::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(narwhalDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	void FirstApp::createPipeline()
	{
		PipelineConfigInfo pipelineConfig{};
		NarwhalPipeline::defaultPipelineConfigInfo(pipelineConfig, narwhalSwapChain.width(), narwhalSwapChain.height());

		pipelineConfig.renderPass = narwhalSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		narwhalPipeline = std::make_unique<NarwhalPipeline>(narwhalDevice, "src/shaders/simple_shader.vert.spv", "src/shaders/simple_shader.frag.spv", pipelineConfig);
		
	}
	void FirstApp::createCommandBuffers()
	{
		commandBuffers.resize(narwhalSwapChain.imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = narwhalDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
		
		if (vkAllocateCommandBuffers(narwhalDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}

		for (int i = 0; i < commandBuffers.size(); ++i) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {	
				throw std::runtime_error("failed to begin recording command buffer!");
			}
			
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = narwhalSwapChain.getRenderPass();
			renderPassInfo.framebuffer = narwhalSwapChain.getFrameBuffer(i);

			renderPassInfo.renderArea.offset = { 0,0 };
			renderPassInfo.renderArea.extent = narwhalSwapChain.getSwapChainExtent();

			
			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { .1f,.1f,.1f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

	
			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); //Start Render pass

			narwhalPipeline->bind(commandBuffers[i]);
			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]); //End render pass
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
			
		}
	}
	void FirstApp::drawFrame(){
		uint32_t imageIndex;
		auto result = narwhalSwapChain.acquireNextImage(&imageIndex);
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		
		result = narwhalSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if  (result != VK_SUCCESS){
			throw std::runtime_error("failed to present swap chain image!");
		}
	}
}