#pragma once

#include "narwhal_device.hpp"
#include "narwhal_buffer.hpp"

#include <vulkan/vulkan_core.h>

//std
#include <string>



namespace narwhal {
	

	class NarwhalImage
	{
	public:
		NarwhalImage(NarwhalDevice& device,std::string name);
		
		
		void loadImage(const std::string& filename);

	private:
		std::string name;

		void createTexureImage(const std::string& filename);
		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

		NarwhalDevice& narwhalDevice;

		int textureWidth, textureHeight, textureChannels;
		VkDeviceSize imageSize;
		
		VkImage image= nullptr;
		VkDeviceMemory imageMemory=nullptr;
	
	};
}

