#pragma once

#include "narwhal_camera.hpp"
#include "narwhal_game_object.hpp"


// lib
#include <vulkan/vulkan.h>

#define _USE_MATH_DEFINES
#include <math.h>

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

	enum class BlackHoleType {
		Schwarzchild,
		Kerr,
	};

	//TODO: set initial values & add kerr parameters
	struct BlackHoleParameters {
		//TODO: Add input textures

		// Black Hole Params
		BlackHoleType blackHoleType = BlackHoleType::Schwarzchild;

		//Step Size Params
		float timeStep= 0.001f;
		float poleMargin = .01f;
		float poleStep = .0001f;
		float escapeDistance = 10000.f;

		//Physical Params
		float horizonRadius = 0.5f;
		float spinFactor = .0f; //KERR SPECIFIC  - RANGE[-1,1]
		float diskMax = 4.f;
		float diskTemp = 1E4F; //RANGE[1E3F,1E4F]
		float innerFalloffRate = 4.5f; //KERR SPECIFIC
		float outerFalloffRate = 2.f; //KERR SPECIFIC
		float beamExponent = 2.f;
		float rotationSpeed = 1.f;
		float timeDelayFactor = 0.1f;
		bool visciousDisk= false; // KERR SPECIFIC
		bool relativeTemp= false; // KERR SPECIFIC

		//Noise Params
		alignas(16) glm::vec3 noiseOffset{ .0f }; 
		float noiseScale = 1.f;
		float noiseCirculation = (float) M_PI_2;
		float noiseH = 1.f;
		int noiseOctaves = 4;

		//Volumetric Noise Params
		float stepSize= 0.01f;
		float absorptionFactor = .5f;
		float noiseCutoff = .5f;
		float noiseMultiplier = 1.f;
		int maxSteps = 20;

		//Brightness Params
		float diskMultiplier = 1.f;
		float starMultiplier = 1.f;

	};

	struct BlackHoleComputeData
	{
		BlackHoleParameters params;
		float time = 0;
		bool hardCheck = false;

	};
}
