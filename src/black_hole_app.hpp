#pragma once

#include "narwhal_window.hpp"
#include "narwhal_device.hpp"
#include "narwhal_renderer.hpp"
#include "narwhal_game_object.hpp"
#include "narwhal_descriptors.hpp"

//std
#include <memory>
#include <vector>


namespace narwhal {
	class BlackHoleApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		BlackHoleApp();
		~BlackHoleApp();

		BlackHoleApp(const BlackHoleApp&) = delete;
		BlackHoleApp& operator=(const BlackHoleApp&) = delete;

		void run();

	private:
		NarwhalWindow narwhalWindow {WIDTH,HEIGHT,"Narwhal Engine V0.1"};
		NarwhalDevice narwhalDevice {narwhalWindow};
		NarwhalRenderer narwhalRenderer {narwhalWindow,narwhalDevice};

		std::unique_ptr<NarwhalDescriptorPool> globalPool {};

		VkFence fence;


	};
}

