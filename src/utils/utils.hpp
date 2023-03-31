#pragma once

#include <string>
#include <vector>
#include <functional>
#include <glm/glm.hpp>


//VK_CHECK from https://vkguide.dev/docs/chapter-1/vulkan_init_code/
#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout <<"Detected Vulkan error: " << err << std::endl; \
			abort();                                                \
		}                                                           \
	} while (0)

namespace narwhal {
	bool readFile(const std::string& filename, std::string& content);
	std::vector<char> readFilePipeline(const std::string& filepath);
	
	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	};
	
}

namespace std {
	template <typename T>
	void hash_combine(size_t& seed, const T& val) {
		seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template<>
	struct hash<glm::vec3> {
		size_t operator()(const glm::vec3& params) const noexcept {
			size_t hash = 0;

			// Hash the enum value
			hash_combine(hash, params.x);
			hash_combine(hash, params.y);
			hash_combine(hash, params.z);
			return hash;
		}
	};
}