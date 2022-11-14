#include "narwhal_window.hpp"

//std
#include <stdexcept>

namespace narwhal {

	NarwhalWindow::NarwhalWindow(int w, int h, std::string name) : width(w), height(h), windowName(name) {
		initWindow();
	}

	NarwhalWindow::~NarwhalWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void NarwhalWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	void NarwhalWindow::initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this); //Gets the pointer to the window
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback); //When window resized, call framebufferResizeCallback
	}

	void NarwhalWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto narwhalWindow= reinterpret_cast<NarwhalWindow*>(glfwGetWindowUserPointer(window));
		narwhalWindow->framebufferResized = true;
		narwhalWindow->width = width;
		narwhalWindow->height = height;
	}
}