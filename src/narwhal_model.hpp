#pragma once

#include "narwhal_device.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <vector>

namespace narwhal {
	class NarwhalModel
	{
	public:

		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;
			
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		NarwhalModel(NarwhalDevice& device, const std::vector<Vertex> &vertices);
		~NarwhalModel();
		
		NarwhalModel(const NarwhalModel&) = delete;
		NarwhalModel& operator=(const NarwhalModel&) = delete;


		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		
		
		
		NarwhalDevice& narwhalDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
		
		
	};
}

