#pragma once

#include <string>
#include <vector>


bool readFile(const std::string& filename, std::string& content);
std::vector<char> readFilePipeline(const std::string& filepath);
