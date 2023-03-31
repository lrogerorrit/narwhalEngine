#pragma once

#include "narwhal_camera.hpp"
#include "narwhal_game_object.hpp"
#include "narwhal_matrix_4.hpp"
#include "utils/utils.hpp"


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
		float timeStep= 0.01f;
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
		bool viscousDisk= false; // KERR SPECIFIC
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
		glm::ivec2 windowSize{ 0 };
	};

	struct BlackHoleFrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		VkDescriptorSet computeDescriptorSet;
		VkFence computeFence;
	};

	struct QuadFrameInfo {
		int frameIndex;
		VkCommandBuffer commandBuffer;
		VkDescriptorSet renderDescriptorSet;
	};

	struct InitFrameInfo {
		int frameIndex;
		VkCommandBuffer commandBuffer;
		VkDescriptorSet initDescriptorSet;
		VkFence computeFence;
	};

	struct InitParameters {
		glm::ivec2 windowSize;
		alignas(16) Matrix44 camToWorld;
		//alignas(16) glm::mat4 camToWorld;
		alignas(16) Matrix44 camInverseProj;
		//alignas(16) glm::mat4 camInverseProj;
		alignas(16)glm::vec3 camPosCartesian;
		alignas(16)glm::vec3 camPosSpherical;
		float horizonRadius;
	};
}


//Hash functions

namespace std {
	template<>
	struct hash<narwhal::BlackHoleParameters> {
		size_t operator()(const narwhal::BlackHoleParameters& params) const noexcept {
			size_t hash = 0;

			// Hash the enum value
			hash_combine(hash, params.blackHoleType);

			// Hash the float values
			hash_combine(hash, params.timeStep);
			hash_combine(hash, params.poleMargin);
			hash_combine(hash, params.poleStep);
			hash_combine(hash, params.escapeDistance);
			hash_combine(hash, params.horizonRadius);
			hash_combine(hash, params.spinFactor);
			hash_combine(hash, params.diskMax);
			hash_combine(hash, params.diskTemp);
			hash_combine(hash, params.innerFalloffRate);
			hash_combine(hash, params.outerFalloffRate);
			hash_combine(hash, params.beamExponent);
			hash_combine(hash, params.rotationSpeed);
			hash_combine(hash, params.timeDelayFactor);
			hash_combine(hash, params.viscousDisk);
			hash_combine(hash, params.relativeTemp);
			hash_combine(hash, params.noiseOffset.x);
			hash_combine(hash, params.noiseOffset.y);
			hash_combine(hash, params.noiseOffset.z);
			hash_combine(hash, params.noiseScale);
			hash_combine(hash, params.noiseCirculation);
			hash_combine(hash, params.noiseH);
			hash_combine(hash, params.noiseOctaves);
			hash_combine(hash, params.stepSize);
			hash_combine(hash, params.absorptionFactor);
			hash_combine(hash, params.noiseCutoff);
			hash_combine(hash, params.noiseMultiplier);
			hash_combine(hash, params.maxSteps);
			hash_combine(hash, params.diskMultiplier);
			hash_combine(hash, params.starMultiplier);

			return hash;
		}
	};
}