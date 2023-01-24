#pragma once

#include "narwhal_device.hpp"
#include "narwhal_buffer.hpp"

#include <vulkan/vulkan_core.h>

//std
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace narwhal {
	

	class NarwhalImage
	{
	public:
		NarwhalImage(NarwhalDevice& device);
		~NarwhalImage();
		
		void loadImage(const std::string& filename);

	private:

		void createTexureImage(const std::string& filename);

		NarwhalDevice& narwhalDevice;

		int textureWidth, textureHeight, textureChannels;
		VkDeviceSize imageSize;
		
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
	
	};
}

