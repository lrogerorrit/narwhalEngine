#pragma once

#include "narwhal_window.hpp"
#include "narwhal_device.hpp"
#include "narwhal_swap_chain.hpp"



//std
#include <memory>
#include <vector>
#include <cassert>


namespace narwhal {
	class NarwhalRenderer
	{

	public:
		

		NarwhalRenderer(NarwhalWindow& window, NarwhalDevice& device);
		~NarwhalRenderer();

		NarwhalRenderer(const NarwhalRenderer&) = delete;
		NarwhalRenderer& operator=(const NarwhalRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return narwhalSwapChain->getRenderPass(); }
		float getAspectRatio() const { return narwhalSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }
		
		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const { 
			assert(isFrameStarted && "Cannot get frame index when frame is not in progress");
			return currentFrameIndex;
		}
		
		
		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
		

		NarwhalWindow& narwhalWindow;
		NarwhalDevice& narwhalDevice;
		std::unique_ptr<NarwhalSwapChain> narwhalSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		
		uint32_t currentImageIndex;
		int currentFrameIndex{0};
		bool isFrameStarted{false};
	};

}

