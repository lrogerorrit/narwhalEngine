#include "narwhal_model.hpp"

//std
#include <cassert>
#include <cstring>


namespace narwhal {
	NarwhalModel::NarwhalModel(NarwhalDevice& device, const std::vector<Vertex>& vertices) :narwhalDevice{ device } {
		createVertexBuffers(vertices);
	}
	NarwhalModel::~NarwhalModel() {
		vkDestroyBuffer(narwhalDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(narwhalDevice.device(), vertexBufferMemory, nullptr);
	}
	

	void NarwhalModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t> (vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3!");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

		narwhalDevice.createBuffer(bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory);
		
		void* data;

		vkMapMemory(narwhalDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize)); //Copies memory from cpu to gpu
		vkUnmapMemory(narwhalDevice.device(), vertexBufferMemory);
	}
	void NarwhalModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}
	void NarwhalModel::draw(VkCommandBuffer commandBuffer) {
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}
	
	std::vector<VkVertexInputBindingDescription> NarwhalModel::Vertex::getBindingDescriptions()
	{
		//could also be:
		// return {{0,sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}
	std::vector<VkVertexInputAttributeDescription> NarwhalModel::Vertex::getAttributeDescriptions()
	{
		return {
			{0,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)}, //Position
			{1,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)} // Color
		};
		
		//std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		//
		//attributeDescriptions[0].binding = 0;
		//attributeDescriptions[0].location = 0; //indicates layout location
		//attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		//attributeDescriptions[0].offset = offsetof(Vertex, position);

		//attributeDescriptions[1].binding = 0;
		//attributeDescriptions[1].location = 1; //indicates layout location
		//attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		//attributeDescriptions[1].offset = offsetof(Vertex, color);
		//
		//return attributeDescriptions;
	}
}