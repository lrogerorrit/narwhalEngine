#include "first_app.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <stdexcept>
#include <array>
#include <iostream>

namespace narwhal {

	struct SimplePushConstantData {
		glm::vec2 offset;
		alignas(16) glm::vec3 color; //See: https://registry.khronos.org/vulkan/specs/1.2/html/chap15.html#interfaces-resources-layout
	};

	FirstApp::FirstApp()
	{
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	FirstApp::~FirstApp()
	{
		vkDestroyPipelineLayout(narwhalDevice.device(), pipelineLayout, nullptr);
	}
	
	void FirstApp::run()
	{
		std::cout << "maxPushConstantSize = " << narwhalDevice.properties.limits.maxPushConstantsSize << std::endl;
		while (!narwhalWindow.shouldClose())
		{
			glfwPollEvents();
			drawFrame();
		}
		
		vkDeviceWaitIdle(narwhalDevice.device());
	}
	void FirstApp::loadModels()
	{
		std::vector<NarwhalModel::Vertex> vertices{
			{{.0f,-.5f},{1.0f,0.0f,0.0f}},
			{{.5f,.5f},{0.0f,1.0f,0.0f}},
			{{-.5f,.5f},{0.0f,0.0f,1.0f}}
		};

		narwhalModel = std::make_unique<NarwhalModel>(narwhalDevice, vertices);
	}
	void FirstApp::createPipelineLayout()
	{
		//Same as  VkPushConstantRange pushConstantRange {VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData)};
		VkPushConstantRange pushConstantRange {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(narwhalDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	void FirstApp::recreateSwapChain(){
		auto extent = narwhalWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) { //If one of window dimensions is 0 (usually in minimization), then pause
			extent = narwhalWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(narwhalDevice.device()); //Wait until current swapchain stops being used
		
		if (narwhalSwapChain == nullptr) {
			narwhalSwapChain = std::make_unique<NarwhalSwapChain>(narwhalDevice, extent);
		}
		else {
			narwhalSwapChain = std::make_unique<NarwhalSwapChain>(narwhalDevice, extent, std::move(narwhalSwapChain));
			if (narwhalSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}

		//TODO: if render pass compatible do nothing else
		createPipeline();
	}

	void FirstApp::createPipeline()
	{
		assert(narwhalSwapChain != nullptr && "Cannot create pipeline before swap chain!");
		assert(pipelineLayout!= nullptr && "Cannot create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		NarwhalPipeline::defaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = narwhalSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		narwhalPipeline = std::make_unique<NarwhalPipeline>(narwhalDevice, "src/shaders/simple_shader.vert.spv", "src/shaders/simple_shader.frag.spv", pipelineConfig);
		
	}
	
	void FirstApp::createCommandBuffers()
	{
		commandBuffers.resize(narwhalSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = narwhalDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
		
		if (vkAllocateCommandBuffers(narwhalDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void FirstApp::freeCommandBuffers() {
		vkFreeCommandBuffers(narwhalDevice.device(), narwhalDevice.getCommandPool(), static_cast<uint32_t> (commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}
	
	void FirstApp::recordCommandBuffer(int imageIndex) {

		static int frame = 0;
		frame = (frame + 1) % 1000;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = narwhalSwapChain->getRenderPass();
		renderPassInfo.framebuffer = narwhalSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = narwhalSwapChain->getSwapChainExtent();


		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f,0.1f,0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();


		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); //Start Render pass

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float> (narwhalSwapChain->getSwapChainExtent().width);
		viewport.height= static_cast<float> (narwhalSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0}, narwhalSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);
		


		narwhalPipeline->bind(commandBuffers[imageIndex]);
		narwhalModel->bind(commandBuffers[imageIndex]);

		for (int j = 0; j < 4; ++j) {
			SimplePushConstantData push{};
			push.offset = { -0.5f+frame*0.002f,-0.4f + j * 0.25f };
			push.color = { .0f,.0f,.2f + .2f * j };

			vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			narwhalModel->draw(commandBuffers[imageIndex]);
		}


		vkCmdEndRenderPass(commandBuffers[imageIndex]); //End render pass
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void FirstApp::drawFrame(){
		uint32_t imageIndex;
		auto result = narwhalSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) { //if current swapchain is no longer valid (e.g. window was resized)
			recreateSwapChain();
			return;
		}
		
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		
		recordCommandBuffer(imageIndex);
		result = narwhalSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || narwhalWindow.wasWindowResized()) {
			narwhalWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}
		if  (result != VK_SUCCESS){
			throw std::runtime_error("failed to present swap chain image!");
		}
	}
}