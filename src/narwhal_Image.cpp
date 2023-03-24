#include "narwhal_image.hpp"
#include "narwhal_buffer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace narwhal {
	NarwhalImage::NarwhalImage(NarwhalDevice& device, std::string path, VkFormat imageFormat)
		: narwhalDevice(device),height(0), width(0), imageFormat(imageFormat), path(path)
	{
		createImage();
		createImageView();
		createSampler();
	}

	NarwhalImage::~NarwhalImage()
	{
		destroy();
	}

	void NarwhalImage::createImage()
	{
		//First we load the image

		int t_width, t_height, t_channels;
		stbi_uc* pixels = stbi_load(path.c_str(), &t_width, &t_height, &t_channels, STBI_rgb_alpha);
		
		height = t_height;
		width = t_width;

		VkDeviceSize imageSize = t_width * t_height * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}
		

		//Create a staging buffer
		NarwhalBuffer stagingBuffer(narwhalDevice, imageSize,1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer.map(imageSize, 0);
		stagingBuffer.writeToBuffer(pixels, imageSize, 0);
		stagingBuffer.unmap();

		//Clear the pixels
		stbi_image_free(pixels);

		//Create the image
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = imageFormat;
		imageInfo.samples= VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(narwhalDevice.device(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image: " + path);
		}
		// Get the memory requirements for the image
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(narwhalDevice.device(), image, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = narwhalDevice.findMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(narwhalDevice.device(), &memoryAllocateInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate image memory: " + path);
		}

		if (vkBindImageMemory(narwhalDevice.device(), image, imageMemory, 0) != VK_SUCCESS) {
			throw std::runtime_error("Failed to bind image: "+path);
		}

		//Transition the image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		narwhalDevice.transitionImageLayout(image, imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		//Copy the buffer to the image
		narwhalDevice.copyBufferToImage(stagingBuffer.getBuffer(), image, width, height,1);
		//Transition the image to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		narwhalDevice.transitionImageLayout(image, imageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		
	}

	void NarwhalImage::createImageView()
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = imageFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(narwhalDevice.device(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image view: "+path);
		}
	}

	void NarwhalImage::createSampler()
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		if (vkCreateSampler(narwhalDevice.device(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create texture sampler: "+path);
		}
	}

	VkDescriptorImageInfo NarwhalImage::getDescriptorImageInfo()
	{
		
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = imageView;
		imageInfo.sampler = sampler;

		return imageInfo;
		
	}



	void NarwhalImage::destroy()
	{
		vkDestroyImageView(narwhalDevice.device(), imageView, nullptr);
		vkDestroyImage(narwhalDevice.device(), image, nullptr);
		vkFreeMemory(narwhalDevice.device(), imageMemory, nullptr);
		vkDestroySampler(narwhalDevice.device(), sampler, nullptr);
	}
}