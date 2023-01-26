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
		~NarwhalImage();
		
		void loadImage(const std::string& filename);

		static VkSampler* textureSampler;
		static void createTextureSampler(NarwhalDevice& device);
		static VkSampler getTextureSampler() { return *textureSampler; };
		
		static std::vector<NarwhalImage*> loadedImages;
		static bool imageExist(std::string fileName);
		//getter for narwhalImage with given name
		static NarwhalImage* getImage(std::string name);
		
		VkImageView getImageView() { return imageView; };
	private:
		std::string name;

		void createTexureImage(const std::string& filename);
		void createTextureImageView();
		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
		
		VkImageView createImageView(VkFormat format);

		NarwhalDevice& narwhalDevice;

		int textureWidth, textureHeight, textureChannels;
		VkDeviceSize imageSize;
		
		VkImage image= nullptr;
		VkImageView imageView = nullptr;
		VkDeviceMemory imageMemory=nullptr;

		
	
	};
}

