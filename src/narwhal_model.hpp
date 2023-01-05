#pragma once

#include "narwhal_device.hpp"
#include "narwhal_buffer.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <memory>
#include <vector>

namespace narwhal {
	class NarwhalModel
	{
	public:

		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};
			
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();


			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		NarwhalModel(NarwhalDevice& device, const NarwhalModel::Builder& builder);
		~NarwhalModel();
		
		NarwhalModel(const NarwhalModel&) = delete;
		NarwhalModel& operator=(const NarwhalModel&) = delete;

		static std::unique_ptr<NarwhalModel> createModelFromFile(NarwhalDevice& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
		
		
		
		NarwhalDevice& narwhalDevice;
		
		std::unique_ptr<NarwhalBuffer> vertexBuffer;
		uint32_t vertexCount;
		
		bool hasIndexBuffer = false;
		std::unique_ptr<NarwhalBuffer> indexBuffer;
		uint32_t indexCount;
		
		
	};
}

