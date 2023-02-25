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
		NarwhalStorageImage(NarwhalDevice& device, uint32_t width, uint32_t height,std::string name="STORAGE_IMAGE");
		~NarwhalStorageImage();

		void createStorageImage(uint32_t width, uint32_t height);
		void createImageView();

		VkImageView getImageView() { return imageView; };
		VkImage getImage() { return image; };
		VkDeviceMemory getImageMemory() { return imageMemory; };

		uint32_t getWidth() { return width; };
		uint32_t getHeight() { return height; };

		void resize(uint32_t width, uint32_t height);

		VkDescriptorImageInfo getDescriptorImageInfo();




		private:

			void destroy();

			NarwhalDevice& narwhalDevice;

			std::string name;
			uint32_t width, height;

			VkImage image = nullptr;
			VkImageView imageView = nullptr;
			VkDeviceMemory imageMemory = nullptr;
	};
}

