#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "narwhal_camera.hpp"
#include "simple_render_system.hpp"
#include "narwhal_buffer.hpp"

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
	//For more alignment info check here:	https://registry.khronos.org/vulkan/specs/1.0-wsi_extensions/html/vkspec.html#interfaces-resources-layout
	//										https://www.oreilly.com/library/view/opengl-programming-guide/9780132748445/app09lev1sec2.html
	struct GlobalUbo {
		glm::mat4 projectionView{ 1.f };
		glm::vec4 ambientLightColor{ 1.f,1.f,1.f,.02f }; //w is intensity
		glm::vec3 lightPosition{ -1.f };
		alignas(16) glm::vec4 lightColor{ 1.f }; // w is light intensity;
		
	};

	FirstApp::FirstApp() {
		globalPool = NarwhalDescriptorPool::Builder(narwhalDevice)
			.setMaxSets(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run()
	{
		std::vector<std::unique_ptr<NarwhalBuffer>> uboBuffers(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<NarwhalBuffer>(narwhalDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}
		
		auto globalSetLayout = NarwhalDescriptorSetLayout::Builder(narwhalDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			NarwhalDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ narwhalDevice, narwhalRenderer.getSwapChainRenderPass(),globalSetLayout->getDescriptorSetLayout()};
		NarwhalCamera camera{};


		auto viewerObject = NarwhalGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
		KeyboardMovementController cameraController{};

		//std::cout << "maxPushConstantSize = " << narwhalDevice.properties.limits.maxPushConstantsSize << std::endl; 

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!narwhalWindow.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			frameTime = glm::min(frameTime, MAX_FRAME_TIME);

			cameraController.moveInPlaneXZ(narwhalWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = narwhalRenderer.getAspectRatio();

			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

			if (auto commandBuffer = narwhalRenderer.beginFrame()) { //Will return a null ptr if swap chain needs to be recreated
				int frameIndex = narwhalRenderer.getFrameIndex();
				FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex],gameObjects};

				//Update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// Render
				narwhalRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
				narwhalRenderer.endSwapChainRenderPass(commandBuffer);
				narwhalRenderer.endFrame();

			}
		}

		vkDeviceWaitIdle(narwhalDevice.device());
	}



	void FirstApp::loadGameObjects()
	{
		std::shared_ptr<NarwhalModel> cubeModel = NarwhalModel::createModelFromFile(narwhalDevice, "data/models/flat_vase.obj");

		auto flatVase = NarwhalGameObject::createGameObject();
		flatVase.model = cubeModel;
		flatVase.transform.translation = { -.5f,.5f,0.f };
		flatVase.transform.scale = glm::vec3({ 3.f,1.5f,3.f });

		gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		cubeModel = NarwhalModel::createModelFromFile(narwhalDevice, "data/models/smooth_vase.obj");
		auto smoothVase = NarwhalGameObject::createGameObject();
		smoothVase.model = cubeModel;
		smoothVase.transform.translation = { .5f,.5f,.0f };
		smoothVase.transform.scale = glm::vec3({ 3.f,1.5f,3.f });

		gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		cubeModel = NarwhalModel::createModelFromFile(narwhalDevice, "data/models/quad.obj");
		auto floor = NarwhalGameObject::createGameObject();
		floor.model = cubeModel;
		floor.transform.translation = {0.f,.5f,0.f };
		floor.transform.scale = glm::vec3({ 3.f,1.f,3.f });

		gameObjects.emplace(floor.getId(), std::move(floor));

	}

}