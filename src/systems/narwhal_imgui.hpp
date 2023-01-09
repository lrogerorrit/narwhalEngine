#pragma once

#include "../narwhal_device.hpp"
#include "../narwhal_window.hpp"
#include "../narwhal_descriptors.hpp"
#include "../narwhal_frame_info.hpp"

// libs
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>



// std
#include <stdexcept>


namespace narwhal {

     static void check_vk_result(VkResult err)
    {
        if (err == 0)
            return;
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        if (err < 0)
            abort();
    }



	class NarwhalImgui
	{

	public:


		NarwhalImgui(NarwhalDevice& device,NarwhalWindow& window, VkRenderPass renderPass, uint32_t imageCount);
		~NarwhalImgui();

		NarwhalImgui(const NarwhalImgui&) = delete;
		NarwhalImgui& operator=(const NarwhalImgui&) = delete;

		void newFrame();

		void render(FrameInfo& frameInfo);
	private:
		
        // Not needed for imgui
		//void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		//void createPipeline(VkRenderPass renderPass);

		NarwhalDevice &narwhalDevice;
        std::unique_ptr<NarwhalDescriptorPool> descriptorPool{};

		

		//std::unique_ptr<NarwhalPipeline> narwhalPipeline;
		//VkPipelineLayout pipelineLayout;
	};

}

