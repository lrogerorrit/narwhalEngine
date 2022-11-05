#pragma once

#include  "narwhal_window.hpp"
#include  "narwhal_pipeline.hpp"

namespace narwhal {
	class FirstApp
	{
	
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		NarwhalWindow window{ WIDTH, HEIGHT, "Hello Vulkan!" };
		narwhalPipeline pipeline{ "src/shaders/simple_shader.vert.spv", "src/shaders/simple_shader.frag.spv" };
	
	};

}

