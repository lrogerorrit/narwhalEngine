#include "narwhal_model.hpp"
#include "utils/utils.hpp"


//libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>



//std
#include <cassert>
#include <cstring>
#include <unordered_map>

namespace std {
	template<>
	struct hash<narwhal::NarwhalModel::Vertex> {
		size_t operator()(narwhal::NarwhalModel::Vertex const& vertex) const {
			size_t seed = 0;
			narwhal::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}


namespace narwhal {
	NarwhalModel::NarwhalModel(NarwhalDevice& device, const NarwhalModel::Builder& builder) :narwhalDevice{ device } {

		// Create Buffers
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
		createMaterialColorBuffers(builder.materials);
		createMaterialIndexBuffers(builder.materialIndices);
		
		// Create Textures
		auto textureOffset = static_cast<uint32_t>(builder.textureNames.size());
		for (auto name : builder.textureNames) {
			std::string path = "data/textures/" + name;
			NarwhalImage texture{ narwhalDevice, path };
			textures.push_back(texture);
		}


	}
	NarwhalModel::~NarwhalModel() {

	}


	void NarwhalModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t> (vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3!");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		NarwhalBuffer stagingBuffer{
			narwhalDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		// Map the buffer memory and copy the data
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		vertexBuffer = std::make_unique<NarwhalBuffer>(
			narwhalDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		// Copy from staging buffer to vertex buffer
		narwhalDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);

	}

	void NarwhalModel::createIndexBuffers(const std::vector<uint32_t>& indices)
	{
		indexCount = static_cast<uint32_t> (indices.size());
		hasIndexBuffer = indexCount > 0;
		if (!hasIndexBuffer) return;

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

		uint32_t indexSize = sizeof(indices[0]);


		NarwhalBuffer stagingBuffer{
			narwhalDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		// Map the buffer memory and copy the data
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		indexBuffer = std::make_unique<NarwhalBuffer>(
			narwhalDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		// Copy from staging buffer to vertex buffer
		narwhalDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}

	void NarwhalModel::createMaterialColorBuffers(const std::vector<MaterialObj>& materials) {
		materialColorCount = static_cast<uint32_t> (materials.size());
		hasMaterialColorBuffer = materialColorCount > 0;
		if (!hasIndexBuffer) return;

		VkDeviceSize bufferSize = sizeof(materials[0]) * materialColorCount;

		uint32_t materialSize = sizeof(materials[0]);


		NarwhalBuffer stagingBuffer{
			narwhalDevice,
			materialSize,
			materialColorCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		// Map the buffer memory and copy the data
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)materials.data());

		materialColorBuffer = std::make_unique<NarwhalBuffer>(
			narwhalDevice,
			materialSize,
			materialColorCount,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		// Copy from staging buffer to materialColor buffer
		narwhalDevice.copyBuffer(stagingBuffer.getBuffer(), materialColorBuffer->getBuffer(), bufferSize);
	}

	void NarwhalModel::createMaterialIndexBuffers(const std::vector<uint32_t>& materialIndexes) {
		materialIndexCount = static_cast<uint32_t> (materialIndexes.size());
		hasMaterialIndexBuffer = materialIndexCount > 0;
		if (!hasIndexBuffer) return;

		VkDeviceSize bufferSize = sizeof(materialIndexes[0]) * materialIndexCount;

		uint32_t materialIndexSize = sizeof(materialIndexes[0]);


		NarwhalBuffer stagingBuffer{
			narwhalDevice,
			materialIndexSize,
			materialIndexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		// Map the buffer memory and copy the data
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)materialIndexes.data());

		materialIndexBuffer = std::make_unique<NarwhalBuffer>(
			narwhalDevice,
			materialIndexSize,
			materialIndexCount,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		// Copy from staging buffer to materialIndexBuffer buffer
		narwhalDevice.copyBuffer(stagingBuffer.getBuffer(), materialIndexBuffer->getBuffer(), bufferSize);
	}

	std::unique_ptr<NarwhalModel> NarwhalModel::createModelFromFile(NarwhalDevice& device, const std::string& filepath)
	{
		Builder builder{};
		builder.loadModel(filepath);
		return std::make_unique<NarwhalModel>(device, builder);
	}

	void NarwhalModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}
	void NarwhalModel::draw(VkCommandBuffer commandBuffer) {
		if (hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
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
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({ 0,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex,position) }); // Position
		attributeDescriptions.push_back({ 1,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex,color) }); // Color
		attributeDescriptions.push_back({ 2,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex,normal) }); // Normal
		attributeDescriptions.push_back({ 3,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex,uv) }); // UV
		//Make material attribute description


		return attributeDescriptions;

	}

	//Code partially from nvidia
	void NarwhalModel::Builder::loadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materialList;
		std::string warn, err;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materialList, &warn, &err, filepath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		//Load obj materials
		for (const auto& material : materialList) {
			MaterialObj m;
			m.ambient = glm::vec3{ material.ambient[0], material.ambient[1], material.ambient[2] };
			m.diffuse = glm::vec3{ material.diffuse[0], material.diffuse[1], material.diffuse[2] };
			m.specular = glm::vec3{ material.specular[0], material.specular[1], material.specular[2] };
			m.emission = glm::vec3{ material.emission[0], material.emission[1], material.emission[2] };
			m.transmittance = glm::vec3{ material.transmittance[0], material.transmittance[1], material.transmittance[2] };
			m.dissolve = material.dissolve;
			m.ior = material.ior;
			m.shininess = material.shininess;
			m.illum = material.illum;

			if (!material.diffuse_texname.empty()) {
				textureNames.push_back(material.diffuse_texname);
				m.textureID = static_cast<int>(textureNames.size()) - 1;

			}
			materials.emplace_back(m);

		}

		if (materials.empty()) {
			materials.emplace_back(MaterialObj{});
		}

		//Converting Data from srgb to linear
		for (auto& material : materials) {
			material.ambient = glm::pow(material.ambient, glm::vec3(2.2f));
			material.diffuse = glm::pow(material.diffuse, glm::vec3(2.2f));
			material.specular = glm::pow(material.specular, glm::vec3(2.2f));
			material.emission = glm::pow(material.emission, glm::vec3(2.2f));
			material.transmittance = glm::pow(material.transmittance, glm::vec3(2.2f));


			vertices.clear();
			indices.clear();

			std::unordered_map<Vertex, uint32_t> uniqueVertices{}; //Vertex, index

			for (const auto& shape : shapes) {
				materialIndices.insert(materialIndices.end(), shape.mesh.material_ids.begin(), shape.mesh.material_ids.end()); // Add material indices
				for (const auto index : shape.mesh.indices) {
					Vertex vertex{};
					if (index.vertex_index >= 0) {
						vertex.position = {
							attrib.vertices[3 * index.vertex_index + 0],
							attrib.vertices[3 * index.vertex_index + 1],
							attrib.vertices[3 * index.vertex_index + 2]
						};

						vertex.color = {
							attrib.colors[3 * index.vertex_index + 0],
							attrib.colors[3 * index.vertex_index + 1],
							attrib.colors[3 * index.vertex_index + 2]
						};
					}
					if (index.normal_index >= 0) {

						vertex.normal = {
							attrib.normals[3 * index.normal_index + 0],
							attrib.normals[3 * index.normal_index + 1],
							attrib.normals[3 * index.normal_index + 2]
						};

					}

					if (index.texcoord_index >= 0) {
						vertex.uv = {
							attrib.texcoords[2 * index.texcoord_index + 0],
							attrib.texcoords[2 * index.texcoord_index + 1]
						};
					}
					else {
						vertex.uv = { 0.0f, 0.0f };
					}
					// If vertex is not in uniqueVertices, add it
					if (uniqueVertices.count(vertex) == 0) {
						uniqueVertices[vertex] = static_cast<uint32_t> (vertices.size());
						vertices.push_back(vertex);
					}
					indices.push_back(uniqueVertices[vertex]);
				}
			}

			// Fixing material indices
			for (auto& mi : materialIndices)
			{
				if (mi < 0 || mi > materials.size())
					mi = 0;
			}

			//Compute normal when no normal is given
			if (attrib.normals.empty()) {
				for (size_t i = 0; i < materialIndices.size(); i += 3) {
					Vertex& v0 = vertices[materialIndices[i + 0]];
					Vertex& v1 = vertices[materialIndices[i + 1]];
					Vertex& v2 = vertices[materialIndices[i + 2]];

					glm::vec3 n = glm::normalize(glm::cross((v1.position - v0.position), (v2.position - v0.position)));
					v0.normal = n;
					v1.normal = n;
					v2.normal = n;
				}
			}


		}

	}
}