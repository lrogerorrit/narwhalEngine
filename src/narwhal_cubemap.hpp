#pragma once
#include "narwhal_device.hpp"
#include "narwhal_buffer.hpp"

#include <vulkan/vulkan.hpp>
//std
#include <string>
#include <stb_image.h>



namespace narwhal {
	class NarwhalCubemap
	{
		public:
		NarwhalCubemap(NarwhalDevice &device, uint32_t width, uint32_t height,std::string&rightFace, std::string&leftFace, std::string&topFace, std::string&bottomFace, std::string&frontFace, std::string&backFace);
			
		~NarwhalCubemap();

		VkDescriptorImageInfo getDescriptorImageInfo();


		private:

			void createCubemapImage(const std::vector<std::string>& faces);
			void createCubemapImageView();
			void createCubemapSampler();

			NarwhalDevice& narwhalDevice;
			uint32_t width, height;

			VkImage image = nullptr;
			VkImageView imageView = nullptr;
			VkDeviceMemory imageMemory = nullptr;
			VkSampler sampler = nullptr;

			std::vector<std::vector<uint8_t>> faceData{};


	};
}

