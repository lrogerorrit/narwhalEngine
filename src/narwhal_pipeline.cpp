#include "narwhal_pipeline.hpp"
#include "utils/utils.hpp"

//std

#include <stdexcept>
#include <iostream>

namespace narwhal {
	
		narwhalPipeline::narwhalPipeline(const std::string & vertFilepath, const std::string & fragFilepath)
		{
			createGraphicsPipeline(vertFilepath, fragFilepath);
		}


		void narwhalPipeline::createGraphicsPipeline(const std::string & vertFilepath, const std::string & fragFilepath)
		{
			// Read in shader files
			std::string vertCode, fragCode;
			if (!readFile(vertFilepath, vertCode) || !readFile(fragFilepath, fragCode))
			{
				throw std::runtime_error("Failed to create graphics pipeline!");
			}


			std::cout << "Vertex shader code size: " << vertCode.size() << std::endl;
			std::cout << "Fragment shader code size: " << fragCode.size() << std::endl;
		}
	
}