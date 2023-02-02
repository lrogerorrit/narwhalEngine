#pragma once

#include "../narwhal_pipeline.hpp"
#include "../narwhal_device.hpp"
#include "../narwhal_camera.hpp"
#include "../narwhal_game_object.hpp"
#include "../narwhal_frame_info.hpp"

//std
#include <memory>
#include <vector>


namespace narwhal {
	class ComputeTestSystem
	{

	public:


		ComputeTestSystem(NarwhalDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~ComputeTestSystem();

		ComputeTestSystem(const ComputeTestSystem&) = delete;
		ComputeTestSystem& operator=(const ComputeTestSystem&) = delete;

		

		void renderGameObjects(FrameInfo& frameInfo);
		void render(FrameInfo& frameInfo);
	private:
		
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		NarwhalDevice &narwhalDevice;
		

		std::unique_ptr<NarwhalPipeline> narwhalPipeline;
		VkPipelineLayout pipelineLayout;
	};

}

