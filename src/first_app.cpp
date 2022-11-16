#include "first_app.hpp"

#include "simple_render_system.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <array>
#include <iostream>

namespace narwhal {

	FirstApp::FirstApp() { loadGameObjects();
	}

	FirstApp::~FirstApp() {}
	
	void FirstApp::run()
	{

		SimpleRenderSystem simpleRenderSystem{ narwhalDevice, narwhalRenderer.getSwapChainRenderPass() };

		std::cout << "maxPushConstantSize = " << narwhalDevice.properties.limits.maxPushConstantsSize << std::endl;
		while (!narwhalWindow.shouldClose())
		{
			glfwPollEvents();
			
			if (auto commandBuffer = narwhalRenderer.beginFrame()) { //Will return a null ptr if swap chain needs to be recreated
				narwhalRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
				narwhalRenderer.endSwapChainRenderPass(commandBuffer);
				narwhalRenderer.endFrame();

			}
		}
		
		vkDeviceWaitIdle(narwhalDevice.device());
	}
	void FirstApp::loadGameObjects()
	{
		std::vector<NarwhalModel::Vertex> vertices{
			{{.0f,-.5f},{1.0f,0.0f,0.0f}},
			{{.5f,.5f},{0.0f,1.0f,0.0f}},
			{{-.5f,.5f},{0.0f,0.0f,1.0f}}
		};

		auto narwhalModel = std::make_shared<NarwhalModel>(narwhalDevice, vertices);
		auto triangle = NarwhalGameObject::createGameObject();
		triangle.model = narwhalModel;
		triangle.color = { .1f,.8f,.1f };
		triangle.transform2d.translation.x = .2f;
		triangle.transform2d.scale = { 2.f,.5f };
		triangle.transform2d.rotation = .25f * glm::two_pi<float>();

		gameObjects.push_back(std::move(triangle));
	}
	
}