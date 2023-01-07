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
	class PointLightSystem
	{

	public:


		PointLightSystem(NarwhalDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		
		void update(FrameInfo& frameInfo, GlobalUbo& ubo);
		void render(FrameInfo& frameInfo);
	private:
		
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		NarwhalDevice &narwhalDevice;
		

		std::unique_ptr<NarwhalPipeline> narwhalPipeline;
		VkPipelineLayout pipelineLayout;
	};

}

