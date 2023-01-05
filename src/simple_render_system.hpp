#pragma once

#include "narwhal_pipeline.hpp"
#include "narwhal_device.hpp"
#include "narwhal_camera.hpp"
#include "narwhal_game_object.hpp"
#include "narwhal_frame_info.hpp"

//std
#include <memory>
#include <vector>


namespace narwhal {
	class SimpleRenderSystem
	{

	public:


		SimpleRenderSystem(NarwhalDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		

		void renderGameObjects(FrameInfo& frameInfo);
	private:
		
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		NarwhalDevice &narwhalDevice;
		

		std::unique_ptr<NarwhalPipeline> narwhalPipeline;
		VkPipelineLayout pipelineLayout;
	};

}

