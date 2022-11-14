#pragma once

#include "narwhal_window.hpp"
#include "narwhal_pipeline.hpp"
#include "narwhal_device.hpp"
#include "narwhal_swap_chain.hpp"
#include "narwhal_model.hpp"

//std
#include <memory>
#include <vector>


namespace narwhal {
	class FirstApp
	{
	
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();

	private:
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();
		
		NarwhalWindow narwhalWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		NarwhalDevice narwhalDevice{ narwhalWindow };
		NarwhalSwapChain narwhalSwapChain{ narwhalDevice, narwhalWindow.getExtent()};
		std::unique_ptr<NarwhalPipeline> narwhalPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<NarwhalModel> narwhalModel;
	
	};

}

