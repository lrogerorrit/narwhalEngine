#include "first_app.hpp"

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

namespace narwhal {

	FirstApp::FirstApp() { loadGameObjects();
	}

	FirstApp::~FirstApp() {}
	
	void FirstApp::run()
	{

		SimpleRenderSystem simpleRenderSystem{ narwhalDevice, narwhalRenderer.getSwapChainRenderPass() };
		NarwhalCamera camera{};
        //camera.setViewDirection(glm::vec3(.0f), glm::vec3(.5f, .0f, 1.f));
		camera.setViewTarget(glm::vec3(-1.f,-2.f,2.f), glm::vec3(0.f, 0.f, 2.5f));

		std::cout << "maxPushConstantSize = " << narwhalDevice.properties.limits.maxPushConstantsSize << std::endl;
		while (!narwhalWindow.shouldClose())
		{
			glfwPollEvents();

            float aspect = narwhalRenderer.getAspectRatio();
		    //camera.setOrthographicProjection(-aspect,aspect,-1,1,-1,1
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
	
    // temporary helper function, creates a 1x1x1 cube centered at offset
    std::unique_ptr<NarwhalModel> createCubeModel(NarwhalDevice& device, glm::vec3 offset) {
        std::vector<NarwhalModel::Vertex> vertices{

            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

        };
        for (auto& v : vertices) {
            v.position += offset;
        }
        return std::make_unique<NarwhalModel>(device, vertices);
    }

	void FirstApp::loadGameObjects()
	{
		std::shared_ptr<NarwhalModel> cubeModel= createCubeModel(narwhalDevice, {.0f,.0f,.0f});
		
        auto cube = NarwhalGameObject::createGameObject();
        cube.model = cubeModel;
        cube.transform.translation = { .0f,.0f,2.5f };
        cube.transform.scale = { .5f,.5f,.5f };

        gameObjects.push_back(std::move(cube));

	}
	
}