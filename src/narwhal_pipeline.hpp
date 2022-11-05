#pragma once

#include <string>
#include <vector>

namespace narwhal {
	class narwhalPipeline
	{
	public:
		narwhalPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
		
	private:
		
		void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
	};
}

