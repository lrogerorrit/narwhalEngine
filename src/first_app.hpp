#pragma once

#include "narwhal_window.hpp"
#include "narwhal_pipeline.hpp"
#include "narwhal_device.hpp"


namespace narwhal {
	class FirstApp
	{
	
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		NarwhalWindow narwhalWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		NarwhalDevice narwhalDevice{ narwhalWindow };
		NarwhalPipeline pipeline{narwhalDevice, "src/shaders/simple_shader.vert.spv", "src/shaders/simple_shader.frag.spv",NarwhalPipeline::defaultPipelineConfigInfo(WIDTH,HEIGHT)};
	
	};

}

