#include "narwhal_storage_image.hpp"


namespace narwhal {
	NarwhalStorageImage::NarwhalStorageImage(NarwhalDevice& device, uint32_t width, uint32_t height, std::string name):narwhalDevice(device), name(name), width(width), height(height)
	{
		createStorageImage(width, height);
		createImageView();

	}
	NarwhalStorageImage::~NarwhalStorageImage()
	{
		destroy();
	}
	void NarwhalStorageImage::createStorageImage(uint32_t width, uint32_t height)
	{
		// Create the image
		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format= VK_FORMAT_R8G8B8A8_UNORM; // TODO: Maybe change in the future so hdr?
		imageCreateInfo.extent.width = width;
		imageCreateInfo.extent.height = height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; // TODO: Maybe remove sampled bit?
		imageCreateInfo.initialLayout= VK_IMAGE_LAYOUT_UNDEFINED;

		if (vkCreateImage(narwhalDevice.device(), &imageCreateInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create texture image with name" + name); 
		}

		// Get the memory requirements for the image
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(narwhalDevice.device(), image, &memoryRequirements);

		// Allocate the memory for the image

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = narwhalDevice.findMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(narwhalDevice.device(), &memoryAllocateInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate texture image memory with name" + name);
		}

		// Bind the memory to the image
		if (vkBindImageMemory(narwhalDevice.device(), image, imageMemory, 0) != VK_SUCCESS) {
			throw std::runtime_error("Failed to bind texture image memory with name" + name);
		}


	}
	void NarwhalStorageImage::createImageView()
	{
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = image;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM; //TODO: Maybe change in the future so hdr?
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(narwhalDevice.device(), &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}
	}
	void NarwhalStorageImage::resize(uint32_t width, uint32_t height)
	{
		if (width == this->width && height == this->height) {
			return;
		}

		destroy();

		this->width = width;
		this->height = height;

		createStorageImage(width, height);
		createImageView();
	}
	VkDescriptorImageInfo NarwhalStorageImage::getDescriptorImageInfo()
	{

		return VkDescriptorImageInfo{
			VK_NULL_HANDLE,
			imageView,
			VK_IMAGE_LAYOUT_GENERAL
		};
	}
	void NarwhalStorageImage::destroy()
	{
		vkDestroyImageView(narwhalDevice.device(), imageView, nullptr);
		vkDestroyImage(narwhalDevice.device(), image, nullptr);
		vkFreeMemory(narwhalDevice.device(), imageMemory, nullptr);
	}
}