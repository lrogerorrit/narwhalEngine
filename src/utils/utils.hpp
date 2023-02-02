#pragma once

#include <string>
#include <vector>
#include <functional>


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