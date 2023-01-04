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

		struct Builder {
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
		};

		NarwhalModel(NarwhalDevice& device, const NarwhalModel::Builder& builder);
		~NarwhalModel();
		
		NarwhalModel(const NarwhalModel&) = delete;
		NarwhalModel& operator=(const NarwhalModel&) = delete;


		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
		
		
		
		NarwhalDevice& narwhalDevice;
		
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
		
		bool hasIndexBuffer = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;
		
		
	};
}

