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

		
		struct MaterialObj {
			glm::vec3 ambient{.1f};
			glm::vec3 diffuse{ .7f };
			glm::vec3 specular{ 1.f };
			glm::vec3 transmittance{ 0.f };
			glm::vec3 emission{ 0.f,0.f,0.1f };
			float shininess{ 0.f };
			float ior{ 1.f }; 	// index of refraction
			float dissolve{ 1.f }; // 1 == opaque; 0 == fully transparent
			// illumination model (see http://www.fileformat.info/format/material/)
			int illum{ 0 };
			int textureID{ -1 };
		};


		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
			std::vector<std::string> textures{};
			std::vector<MaterialObj> materials{};
			std::vector<uint32_t> materialIndices{};
			

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

		void createMaterialColorBuffers(const std::vector<MaterialObj>& materials);

		void createMaterialIndexBuffers(const std::vector<uint32_t>& materialIndexes);
		
		
		
		NarwhalDevice& narwhalDevice;
		
		std::unique_ptr<NarwhalBuffer> vertexBuffer;
		uint32_t vertexCount;
		
		bool hasIndexBuffer = false;
		std::unique_ptr<NarwhalBuffer> indexBuffer;
		uint32_t indexCount;
		
		bool hasMaterialColorBuffer = false;
		std::unique_ptr<NarwhalBuffer> materialColorBuffer;
		uint32_t materialColorCount;
		
		bool hasMaterialIndexBuffer = false;
		std::unique_ptr<NarwhalBuffer> materialIndexBuffer;
		uint32_t materialIndexCount;
		
		
		
	};
}

