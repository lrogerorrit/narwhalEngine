#include "utils.hpp"
#include <iostream>

//std
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <vector>

namespace narwhal {

	bool readFile(const std::string& filename, std::string& content)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		//check if file doesnt exist and throw error
		if (!file.is_open())
		{
			std::cout << "Failed to open file: " << filename << std::endl;
			return false;
		}

		size_t fileSize = (size_t)file.tellg();
		content.resize(fileSize);

		file.seekg(0);
		file.read(content.data(), fileSize);

		file.close();
		return true;
	}

	std::vector<char> readFilePipeline(const std::string& filepath)
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file: " + filepath);
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}
}