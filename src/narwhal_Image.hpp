#pragma once
#include "narwhal_device.hpp"

#include <vulkan/vulkan.hpp>

//std
#include <string>


namespace narwhal {
	class NarwhalImage
	{
	public:
		NarwhalImage(NarwhalDevice& device, std::string path, VkFormat imageFormat= VK_FORMAT_R8G8B8A8_SRGB);
		~NarwhalImage();

		void createImage();
		void createImageView();
		void createSampler();

		VkDescriptorImageInfo getDescriptorImageInfo();
	private:
		
		
		void destroy();
		std::string path;

		NarwhalDevice& narwhalDevice;

		uint32_t width, height;

		VkFormat imageFormat;

		VkImage image = nullptr;
		VkImageView imageView = nullptr;
		VkDeviceMemory imageMemory = nullptr;
		VkSampler sampler = nullptr;
	};
}

