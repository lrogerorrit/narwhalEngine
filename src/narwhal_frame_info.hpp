#pragma once

#include "narwhal_camera.hpp"
#include "narwhal_game_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace narwhal {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		NarwhalCamera camera;
		VkDescriptorSet globalDescriptorSet;
		NarwhalGameObject::Map& gameObjects;
	};
}
