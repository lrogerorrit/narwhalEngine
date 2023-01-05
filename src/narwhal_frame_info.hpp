#pragma once

#include "narwhal_camera.hpp"

// lib
#include <vulkan/vulkan.h>

namespace narwhal {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		NarwhalCamera camera;
		VkDescriptorSet globalDescriptorSet;
	};
}
