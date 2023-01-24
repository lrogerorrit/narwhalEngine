#include "narwhal_Image.hpp"


//Std
#include <stdexcept>

namespace narwhal {
	NarwhalImage::NarwhalImage(NarwhalDevice& device) : narwhalDevice{ device }, 
		textureHeight{ 0 },textureWidth{ 0 },textureChannels{ 0 }, imageSize{0}
	{
	
	}
	void NarwhalImage::loadImage(const std::string& filename)
	{
		createTexureImage(filename);
	}
	
	void NarwhalImage::createTexureImage(const std::string& filename)
	{
		//The STBI_rgb_alpha value forces the image to be loaded with an alpha channel, even if it doesn't have one, 
		//which is nice for consistency with other textures in the future.
		stbi_uc* pixels = stbi_load(filename.c_str(), &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
		imageSize = textureWidth * textureHeight * 4; // We multiply by 4 since we have 4 channels (rgba)
		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		//Create a staging buffer
		NarwhalBuffer stagingBuffer{
			narwhalDevice,
			imageSize,
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)pixels);
		
		stbi_image_free(pixels); // Free the pixels since we don't need them anymore
		
		
		//Create the texture image
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t> (textureWidth);
		imageInfo.extent.height = static_cast<uint32_t> (textureHeight);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1; //TODO: Change to support mipmaps
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0; // Optional
		
		narwhalDevice.createImageWithInfo(
			imageInfo,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			textureImage,
			textureImageMemory
		);
		

		
	}
}