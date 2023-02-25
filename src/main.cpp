#include "black_hole_app.hpp"

//std
#include <iostream>
#include <cstdlib>
#include <stdexcept>

int main() {

	try {
		narwhal::BlackHoleApp app{};
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}