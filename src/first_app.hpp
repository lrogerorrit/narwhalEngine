#pragma once

#include "narwhal_window.hpp"
#include "narwhal_device.hpp"
#include "narwhal_renderer.hpp"

#include "narwhal_game_object.hpp"

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
		void loadGameObjects();
		
		NarwhalWindow narwhalWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		NarwhalDevice narwhalDevice{ narwhalWindow };
		NarwhalRenderer narwhalRenderer{ narwhalWindow, narwhalDevice };
			
		std::vector<NarwhalGameObject> gameObjects;
	
	};

}

