#include "narwhal_cubemap.hpp"
#include "narwhal_pipeline.hpp"

namespace narwhal {
	NarwhalCubemap::NarwhalCubemap(NarwhalDevice& device, uint32_t width, uint32_t height, std::string& rightFace, std::string& leftFace, std::string& topFace, std::string& bottomFace, std::string& frontFace, std::string& backFace)
		:narwhalDevice(device), width(width), height(height)
	{
		std::vector<std::string> faces={rightFace,leftFace,topFace,bottomFace,frontFace,backFace};
		createCubemapImage(faces);
		createCubemapImageView();
		createCubemapSampler();
		
	}
	NarwhalCubemap::~NarwhalCubemap()
	{
		vkDestroyImageView(narwhalDevice.device(), imageView, nullptr);
		vkDestroyImage(narwhalDevice.device(), image, nullptr);
		vkFreeMemory(narwhalDevice.device(), imageMemory, nullptr);
		vkDestroySampler(narwhalDevice.device(), sampler, nullptr);
	}
	VkDescriptorImageInfo NarwhalCubemap::getDescriptorImageInfo()
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = imageView;
		imageInfo.sampler = sampler;

		return imageInfo;
	}
	void NarwhalCubemap::createCubemapImage(const std::vector<std::string>& faces)
	{
		int numFaces= faces.size();
		faceData.resize(numFaces);

		// Load the image data
		for (int i = 0; i < numFaces; i++) {
			int width, height, channels;
			stbi_uc* pixels = stbi_load(faces[i].c_str(), &width, &height, &channels, STBI_rgb_alpha);
			size_t imageSize = width * height * 4;

			if (!pixels) {
				throw std::runtime_error("Failed to load texture image!");
			}

			faceData[i].resize(imageSize);
			memcpy(faceData[i].data(), pixels, static_cast<size_t>(imageSize));

			stbi_image_free(pixels);
		}


		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageCreateInfo.extent.width = width;
		imageCreateInfo.extent.height = height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 6;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		if (vkCreateImage(narwhalDevice.device(), &imageCreateInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image");
		}

		// Get the memory requirements for the image
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(narwhalDevice.device(), image, &memoryRequirements);

		// Allocate memory for the image
		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = narwhalDevice.findMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(narwhalDevice.device(), &memoryAllocateInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate image memory");
		}

		// Bind the memory to the image
		vkBindImageMemory(narwhalDevice.device(), image, imageMemory, 0);
		
		//Load the cube faces



		
		
		
		for (int i = 0; i < faces.size(); i++)
		{

			VkCommandBuffer commandBuffer = narwhalDevice.beginSingleTimeCommands();
			NarwhalBuffer stagingBuffer{ narwhalDevice,faceData[i].size(),1,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
			stagingBuffer.map();
			stagingBuffer.writeToBuffer(faceData[i].data(), faceData[i].size());

			//We now prepare the image for copying by transitioning it to the VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL layout.
			
			narwhalDevice.transitionImageLayout(commandBuffer, image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, i, 1);
			narwhalDevice.copyBufferToImage(commandBuffer,stagingBuffer.getBuffer(), image, width, height, 1, i);

			//We then transition the image to the VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL layout, which is the layout that we will use to sample from the image in the shader.
			stagingBuffer.unmap();
			narwhalDevice.endSingleTimeCommands(commandBuffer);
		}
			narwhalDevice.transitionImageLayout(image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,0,6);
	}
	void NarwhalCubemap::createCubemapImageView()
	{
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = image;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM; //TODO: Maybe change in the future so hdr?
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 6;

		if (vkCreateImageView(narwhalDevice.device(), &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}
	}
	void NarwhalCubemap::createCubemapSampler()
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
			throw std::runtime_error("Failed to create texture sampler!");
		}

	}
}