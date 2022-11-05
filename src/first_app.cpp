#include "first_app.hpp"

namespace narwhal {
	void FirstApp::run()
	{
		while (!narwhalWindow.shouldClose())
		{
			glfwPollEvents();
		}
	}
}