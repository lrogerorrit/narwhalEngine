#include "black_hole_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "narwhal_camera.hpp"
#include "narwhal_buffer.hpp"
#include "narwhal_storage_image.hpp"

#include "systems/narwhal_imgui.hpp"



//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

//std
#include <stdexcept>
#include <array>
#include <iostream>
#include <chrono>

#define MAX_FRAME_TIME 1.f //TODO: Change and tune


namespace narwhal {
	//For more alignment info check here:	https://registry.khronos.org/vulkan/specs/1.0-wsi_extensions/html/vkspec.html#interfaces-resources-layout
	//										https://www.oreilly.com/library/view/opengl-programming-guide/9780132748445/app09lev1sec2.html

	BlackHoleApp::BlackHoleApp() {
		const int POOL_SETS_COUNT = 3;
		globalPool = NarwhalDescriptorPool::Builder(narwhalDevice)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.setMaxSets(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT* POOL_SETS_COUNT)
			.build();

		//Load Fence
		VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
		vkCreateFence(narwhalDevice.device(), &fenceInfo, nullptr, &fence);
	}


	BlackHoleApp::~BlackHoleApp() {
		vkDestroyFence(narwhalDevice.device(), fence, nullptr);
	}

	void BlackHoleApp::run()
	{
		//SETUP
		// create imgui, and pass in dependencies
		NarwhalImgui narwhalImgui{
			narwhalDevice,
			narwhalWindow,
			narwhalRenderer.getSwapChainRenderPass(),
			narwhalRenderer.getImageCount() };

		// Make Buffers
		std::vector<std::unique_ptr<NarwhalBuffer>> parameterBuffers( NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		std::vector<std::unique_ptr<NarwhalBuffer>> uboBuffers( NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		// Make Storage Images
		std::vector<std::unique_ptr<NarwhalStorageImage>> storageImages( NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
			
		}


	}


}