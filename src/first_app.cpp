#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "narwhal_camera.hpp"
#include "narwhal_buffer.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"
#include "systems/narwhal_imgui.hpp"
#include "systems/compute_shader_test.hpp"


//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

//std
#include <stdexcept>
#include <array>
#include <iostream>
#include <chrono>


#define MAX_FRAME_TIME 1.f //TODO: Change and tune

namespace narwhal {
	//For more alignment info check here:	https://registry.khronos.org/vulkan/specs/1.0-wsi_extensions/html/vkspec.html#interfaces-resources-layout
	//										https://www.oreilly.com/library/view/opengl-programming-guide/9780132748445/app09lev1sec2.html
	
	

	FirstApp::FirstApp() {
		globalPool = NarwhalDescriptorPool::Builder(narwhalDevice)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.setMaxSets(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT*2)
			
			

			
			.build();
		
			
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run()
	{
		// create imgui, and pass in dependencies
		NarwhalImgui narwhalImgui{
			narwhalDevice,
			narwhalWindow,
			narwhalRenderer.getSwapChainRenderPass(),
			narwhalRenderer.getImageCount() };
		
		std::vector<std::unique_ptr<NarwhalBuffer>> uboBuffers(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		std::vector<std::unique_ptr<NarwhalBuffer>> storageBuffers(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		
		
		const int MAX_OBJECTS = 10000;
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<NarwhalBuffer>(narwhalDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			
			uboBuffers[i]->map();
		}
		for (int i = 0; i < storageBuffers.size(); i++) {
			storageBuffers[i] = std::make_unique<NarwhalBuffer>(narwhalDevice, sizeof(ComputeTestData)*MAX_OBJECTS, 1, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); //Needs to be coherent since if we want to write from the gpu, then we need to 
			storageBuffers[i]->map();
			
		}
		
		auto globalSetLayout = NarwhalDescriptorSetLayout::Builder(narwhalDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		auto computeSetLayout = NarwhalDescriptorSetLayout::Builder(narwhalDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
			.build();
			
		//TODO: write to computesetLayout
		
		std::vector<VkDescriptorSet> globalDescriptorSets(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		std::vector<VkDescriptorSet> computeDescriptorSets(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {


			
			auto uboBufferInfo = uboBuffers[i]->descriptorInfo();
			
			NarwhalDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &uboBufferInfo)
				
				.build(globalDescriptorSets[i]);
		}

		for (int i = 0; i < computeDescriptorSets.size(); i++) {
			
			auto storageBufferInfo = storageBuffers[i]->descriptorInfo();
			

			NarwhalDescriptorWriter(*computeSetLayout, *globalPool)
				.writeBuffer(0, &storageBufferInfo)
				.build(computeDescriptorSets[i]);
		}

		ComputeTestSystem computeTestSystem{ narwhalDevice, narwhalRenderer.getSwapChainRenderPass(), computeSetLayout->getDescriptorSetLayout()};

		//SimpleRenderSystem simpleRenderSystem{ narwhalDevice, narwhalRenderer.getSwapChainRenderPass(),globalSetLayout->getDescriptorSetLayout()};
		PointLightSystem pointLightSystem{ narwhalDevice, narwhalRenderer.getSwapChainRenderPass(),globalSetLayout->getDescriptorSetLayout() };
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

			narwhalImgui.newFrame();

			cameraController.moveInPlaneXZ(narwhalWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = narwhalRenderer.getAspectRatio();

			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

			if (auto commandBuffer = narwhalRenderer.beginFrame()) { //Will return a null ptr if swap chain needs to be recreated
				int frameIndex = narwhalRenderer.getFrameIndex();
				FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex],computeDescriptorSets[frameIndex],gameObjects};

				//Update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view= camera.getView();
				ubo.inverseView = camera.getInverseView();
				pointLightSystem.update(frameInfo,ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// Render
				computeTestSystem.render(frameInfo);
				narwhalRenderer.beginSwapChainRenderPass(commandBuffer);
				//simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
				

				//TODO: REMOVE
				ImGui::Begin("Test");
				ImGui::Text("Hello World");
				ImGui::End();
				
				narwhalImgui.render(frameInfo);

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

				
		std::vector<glm::vec3> lightColors{
		 {1.f, .1f, .1f},
		 {.1f, .1f, 1.f},
		 {.1f, 1.f, .1f},
		 {1.f, 1.f, .1f},
		 {.1f, 1.f, 1.f},
		 {1.f, 1.f, 1.f}  //
		};
		
		for (int i = 0; i < lightColors.size(); i++) {
			if (i >= MAX_LIGHTS) break;
			auto pointLight = NarwhalGameObject::makePointLight(0.2);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>()) / lightColors.size(), glm::vec3(0.f, -1.f, 0.f));
			
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f,-1.f,-1.f,1.f));
			
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
		

	}

}