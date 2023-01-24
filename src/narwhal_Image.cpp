#include "narwhal_image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//Std
#include <stdexcept>

namespace narwhal {
	NarwhalImage::NarwhalImage(NarwhalDevice& device, std::string name) : narwhalDevice{ device },
		textureHeight{ 0 },textureWidth{ 0 },textureChannels{ 0 }, imageSize{0}
	{
		image = nullptr;
		imageMemory = nullptr;
		this->name = name;
	}
	NarwhalImage::~NarwhalImage()
	{
		vkDestroySampler(narwhalDevice.device(), textureSampler, nullptr);
		vkDestroyImageView(narwhalDevice.device(), imageView, nullptr);
		vkDestroyImage(narwhalDevice.device() ,image, nullptr);
		vkFreeMemory(narwhalDevice.device(), imageMemory, nullptr);
		
		
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
		
		createImage(
			textureWidth,
			textureHeight,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		//We now prepare the image for copying by transitioning it to the VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL layout.
		narwhalDevice.transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		//We then copy the buffer to the image.
		narwhalDevice.copyBufferToImage(stagingBuffer.getBuffer(), image, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight),1);
		//We then transition the image to the VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL layout, which is the layout that we will use to sample from the image in the shader.
		narwhalDevice.transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
	void NarwhalImage::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties )
	{
		VkImageCreateInfo imageInfo{};
		
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
		narwhalDevice.createImageWithInfo(
			imageInfo,
			properties,
			image,
			imageMemory
		);
	}

	void NarwhalImage::createTextureSampler()
	{

		
		

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		
		samplerInfo.anisotropyEnable = narwhalDevice.features.samplerAnisotropy?VK_TRUE:VK_FALSE;
		samplerInfo.maxAnisotropy = narwhalDevice.properties.limits.maxSamplerAnisotropy ;
		
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		//TODO: Add mipmap functionality
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(narwhalDevice.device(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}

	}

	VkImageView NarwhalImage::createImageView(VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		VkImageView imageView;

		if (vkCreateImageView(narwhalDevice.device(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}
		return imageView;
	}
	
	void NarwhalImage::createTextureImageView(VkFormat format)	{
		this->imageView = createImageView(format, VK_FORMAT_R8G8B8A8_SRGB);
		
		
	}
}