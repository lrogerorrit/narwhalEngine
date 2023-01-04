#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "narwhal_camera.hpp"
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
#include <chrono>

#define MAX_FRAME_TIME 1.f //TODO: Change and tune

namespace narwhal {

	FirstApp::FirstApp() { loadGameObjects();
	}

	FirstApp::~FirstApp() {}
	
	void FirstApp::run()
	{

		SimpleRenderSystem simpleRenderSystem{ narwhalDevice, narwhalRenderer.getSwapChainRenderPass() };
		NarwhalCamera camera{};


		auto viewerObject = NarwhalGameObject::createGameObject();
        KeyboardMovementController cameraController{};

		//std::cout << "maxPushConstantSize = " << narwhalDevice.properties.limits.maxPushConstantsSize << std::endl; 
        
		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!narwhalWindow.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime= std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

            frameTime = glm::min(frameTime, MAX_FRAME_TIME);
            
            cameraController.moveInPlaneXZ(narwhalWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            
            float aspect = narwhalRenderer.getAspectRatio();
	
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
			
			if (auto commandBuffer = narwhalRenderer.beginFrame()) { //Will return a null ptr if swap chain needs to be recreated
				narwhalRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects,camera);
				narwhalRenderer.endSwapChainRenderPass(commandBuffer);
				narwhalRenderer.endFrame();

			}
		}
		
		vkDeviceWaitIdle(narwhalDevice.device());
	}
	
    

	void FirstApp::loadGameObjects()
	{
		std::shared_ptr<NarwhalModel> cubeModel = NarwhalModel::createModelFromFile(narwhalDevice, "data/models/smooth_vase.obj");
		
        auto gameObj = NarwhalGameObject::createGameObject();
		gameObj.model = cubeModel;
		gameObj.transform.translation = { .0f,.0f,2.5f };
		gameObj.transform.scale = glm::vec3(3.f);

        gameObjects.push_back(std::move(gameObj));

	}
	
}