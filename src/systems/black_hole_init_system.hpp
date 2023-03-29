#pragma once

#include "../narwhal_pipeline.hpp"
#include "../narwhal_device.hpp"
#include "../narwhal_camera.hpp"
#include "../narwhal_frame_info.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <memory>
#include <vector>


namespace narwhal {
	class BlackHoleInitSystem
	{
	public:

		BlackHoleInitSystem(NarwhalDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout setLayout);
		~BlackHoleInitSystem();

		BlackHoleInitSystem(const BlackHoleInitSystem&) = delete; // Remove copy constructor
		BlackHoleInitSystem& operator=(const BlackHoleInitSystem&) = delete; // Remove copy assignment operator

		void initFrame(InitFrameInfo& frameInfo, VkExtent2D& size);

	private:
		void createPipelineLayout(VkDescriptorSetLayout setLayout);
		void createPipelines(VkRenderPass renderPass);

		NarwhalDevice &narwhalDevice;

		std::unique_ptr<NarwhalPipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};
}

