#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace narwhal {
	class NarwhalWindow {
	public:
		NarwhalWindow(int w, int h, std::string name);
		~NarwhalWindow();

		NarwhalWindow(const NarwhalWindow&) = delete;
		NarwhalWindow& operator=(const NarwhalWindow&) = delete;
		
		bool shouldClose() { return glfwWindowShouldClose(window); }
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		void initWindow();
		
		const int width;
		const int height;
		

		std::string windowName;
		

		GLFWwindow* window;
	};

}
