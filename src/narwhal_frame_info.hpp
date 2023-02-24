#pragma once

#include "narwhal_camera.hpp"
#include "narwhal_game_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace narwhal {
	
	#define MAX_LIGHTS 10
	
	struct PointLight {
		glm::vec3 position{};
		alignas(16) glm::vec4 color{}; // w is intensity
		
	};

	struct GlobalUbo {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };
		glm::vec4 ambientLightColor{ 1.f,1.f,1.f,.02f }; //w is intensity
		PointLight pointLights[MAX_LIGHTS];
		int numLights;

	};
	
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		NarwhalCamera camera;
		VkDescriptorSet globalDescriptorSet;
		VkDescriptorSet computeDescriptorSet;
		NarwhalGameObject::Map& gameObjects;
		VkFence computeFence;
		
	};

	struct ComputeTestData {
		glm::vec3 pixelData;
	};
}
