#pragma once

#include "narwhal_window.hpp"
#include "narwhal_device.hpp"
#include "narwhal_renderer.hpp"
#include "narwhal_game_object.hpp"
#include "narwhal_descriptors.hpp"
#include "narwhal_frame_info.hpp"


//std
#include <memory>
#include <vector>


namespace narwhal {

	class NarwhalImgui;
	class BlackHoleApp
	{
	public:
		static constexpr int WIDTH = 1920*.9;
		static constexpr int HEIGHT = 1080*.9;

		BlackHoleApp();
		~BlackHoleApp();

		BlackHoleApp(const BlackHoleApp&) = delete;
		BlackHoleApp& operator=(const BlackHoleApp&) = delete;

		void run();
		


	private:

		void renderImgui(NarwhalImgui& narwhalImgui, VkCommandBuffer commandBuffer);


		NarwhalWindow narwhalWindow {WIDTH,HEIGHT,"Narwhal Engine V0.1"};
		NarwhalDevice narwhalDevice {narwhalWindow};
		NarwhalRenderer narwhalRenderer {narwhalWindow,narwhalDevice};

		std::unique_ptr<NarwhalDescriptorPool> globalPool {};

		VkFence fence;
		BlackHoleComputeData computeData;
		BlackHoleParameters blackHoleParameters;
		InitParameters initParameters;

		bool showImgui = true;
		float schwarzchildFrameThreshold = .99f;
		float kerrFrameThreshold = .8f;
		int percentageCheckInterval = 0;

	};
}

