#pragma once

#include "../narwhal_pipeline.hpp"
#include "../narwhal_device.hpp"
#include "../narwhal_camera.hpp"
#include "../narwhal_game_object.hpp"
#include "../narwhal_frame_info.hpp"
#include "../narwhal_model.hpp"

//std
#include <memory>
#include <vector>

namespace narwhal {
	class QuadRenderSystem
	{

	public:


		QuadRenderSystem(NarwhalDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout setLayout);
		~QuadRenderSystem();

		QuadRenderSystem(const QuadRenderSystem&) = delete;
		QuadRenderSystem& operator=(const QuadRenderSystem&) = delete;

		

		void render(QuadFrameInfo& frameInfo);

	private:

		void createPipelineLayout(VkDescriptorSetLayout setLayout);
		void createPipelines(VkRenderPass renderPass);

		NarwhalDevice& narwhalDevice;
		std::unique_ptr<NarwhalPipeline> pipeline;
		std::shared_ptr<NarwhalModel> quadModel;
		VkPipelineLayout pipelineLayout;
	};

}
