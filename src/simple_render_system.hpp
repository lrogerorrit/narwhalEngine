#pragma once

#include "narwhal_pipeline.hpp"
#include "narwhal_device.hpp"
#include "narwhal_camera.hpp"
#include "narwhal_game_object.hpp"

//std
#include <memory>
#include <vector>


namespace narwhal {
	class SimpleRenderSystem
	{

	public:


		SimpleRenderSystem(NarwhalDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<NarwhalGameObject>& gameObjects, const NarwhalCamera& camera);
	private:
		
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		NarwhalDevice &narwhalDevice;
		

		std::unique_ptr<NarwhalPipeline> narwhalPipeline;
		VkPipelineLayout pipelineLayout;
	};

}

