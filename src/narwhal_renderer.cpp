#include "narwhal_renderer.hpp"



//std
#include <stdexcept>
#include <array>
#include <iostream>

namespace narwhal {

	NarwhalRenderer::NarwhalRenderer(NarwhalWindow& window, NarwhalDevice& device) : narwhalWindow{ window }, narwhalDevice{ device } {
		recreateSwapChain();
		createCommandBuffers();
	}

	NarwhalRenderer::~NarwhalRenderer() { freeCommandBuffers(); }

	void NarwhalRenderer::recreateSwapChain() {
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
			std::shared_ptr<NarwhalSwapChain> oldSwapChain = std::move(narwhalSwapChain);
			narwhalSwapChain = std::make_unique<NarwhalSwapChain>(narwhalDevice, extent, oldSwapChain);
			
			if (!oldSwapChain->compareSwapFormats(*narwhalSwapChain.get())) {
				throw std::runtime_error("Swapchain image format or color space has changed!");
			}

			
			
		}


		
		
	}


	void NarwhalRenderer::createCommandBuffers()
	{
		commandBuffers.resize(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);

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

	void NarwhalRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(narwhalDevice.device(), narwhalDevice.getCommandPool(), static_cast<uint32_t> (commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}
	VkCommandBuffer NarwhalRenderer::beginFrame() {
		assert(!isFrameStarted && "Cant call beginFrame while frame already in progress");
		
		
		auto result = narwhalSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) { //if current swapchain is no longer valid (e.g. window was resized)
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}
	void NarwhalRenderer::endFrame(){
		bool windowWasResized = false;
		assert(isFrameStarted && "Cant call endFrame while frame not in progress");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = narwhalSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || narwhalWindow.wasWindowResized()) {
			narwhalWindow.resetWindowResizedFlag();
			windowWasResized = true;
			recreateSwapChain();
		}
		if (result != VK_SUCCESS && !windowWasResized) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT;

	}
	void NarwhalRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cant call beginSwapChainRenderPass while frame not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cant begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = narwhalSwapChain->getRenderPass();
		renderPassInfo.framebuffer = narwhalSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = narwhalSwapChain->getSwapChainExtent();


		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f,0.01f,0.01f, 1.0f }; // Background color
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();


		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); //Start Render pass

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float> (narwhalSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float> (narwhalSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0}, narwhalSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}
	void NarwhalRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cant call endSwapChainRenderPass while frame not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cant end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer); //End render pass


	}
}