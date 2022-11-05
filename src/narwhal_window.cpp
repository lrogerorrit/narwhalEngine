#include "narwhal_window.hpp"

namespace narwhal {

	NarwhalWindow::NarwhalWindow(int w, int h, std::string name) : width(w), height(h), windowName(name) {
		initWindow();
	}

	NarwhalWindow::~NarwhalWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void NarwhalWindow::initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}

}