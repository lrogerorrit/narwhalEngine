#pragma once
#include "narwhal_device.hpp"
#include "narwhal_buffer.hpp"

#include <vulkan/vulkan.hpp>

//std
#include <string>


namespace narwhal {
	class NarwhalStorageImage
	{
		public:
		NarwhalStorageImage(NarwhalDevice& device, std::string name, uint32_t width, uint32_t height);
		~NarwhalStorageImage();

		void createStorageImage(uint32_t width, uint32_t height);
		void createImageView();

		VkImageView getImageView() { return imageView; };
		VkImage getImage() { return image; };
		VkDeviceMemory getImageMemory() { return imageMemory; };

		uint32_t getWidth() { return width; };
		uint32_t getHeight() { return height; };



		private:

			NarwhalDevice& narwhalDevice;

			std::string name;
			uint32_t width, height;

			VkImage image = nullptr;
			VkImageView imageView = nullptr;
			VkDeviceMemory imageMemory = nullptr;
	};
}

