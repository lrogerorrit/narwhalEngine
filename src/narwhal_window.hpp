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
		VkExtent2D getExtent() { return { static_cast<uint32_t> (width),  static_cast<uint32_t> (height) }; }
		
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		void initWindow();
		
		const int width;
		const int height;
		

		std::string windowName;
		

		GLFWwindow* window;
	};

}
