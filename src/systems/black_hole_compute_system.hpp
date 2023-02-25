#pragma once

#include "../narwhal_pipeline.hpp"
#include "../narwhal_device.hpp"
#include "../narwhal_camera.hpp"
#include "../narwhal_frame_info.hpp"

//std
#include <memory>
#include <vector>


namespace narwhal {
	class BlackHoleComputeSystem
	{
	public:

		BlackHoleComputeSystem(NarwhalDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout setLayout);
		~BlackHoleComputeSystem();

		BlackHoleComputeSystem(const BlackHoleComputeSystem&) = delete; // Remove copy constructor
		BlackHoleComputeSystem& operator=(const BlackHoleComputeSystem&) = delete; // Remove copy assignment operator

		void render(FrameInfo& frameInfo, BlackHoleParameters& parameters);

	private:
		void createPipelineLayout(VkDescriptorSetLayout setLayout);
		void createPipelines(VkRenderPass renderPass);

		NarwhalDevice &narwhalDevice;

		std::unique_ptr<NarwhalPipeline> schwarzchildPipeline;
		std::unique_ptr<NarwhalPipeline> kerrPipeline;
		VkPipelineLayout pipelineLayout;
	};
}

