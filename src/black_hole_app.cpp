#include "black_hole_app.hpp"

#include "narwhal_camera.hpp"
#include "narwhal_buffer.hpp"
#include "narwhal_storage_image.hpp"

#include "systems/narwhal_imgui.hpp"



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

#define MAX_DT 1.f //TODO: Change and tune


namespace narwhal {
	//For more alignment info check here:	https://registry.khronos.org/vulkan/specs/1.0-wsi_extensions/html/vkspec.html#interfaces-resources-layout
	//										https://www.oreilly.com/library/view/opengl-programming-guide/9780132748445/app09lev1sec2.html

	BlackHoleApp::BlackHoleApp() {
		const int POOL_SETS_COUNT = 6;
		globalPool = NarwhalDescriptorPool::Builder(narwhalDevice)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.setMaxSets(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT * POOL_SETS_COUNT)
			.build();

		//Load Fence
		VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		vkCreateFence(narwhalDevice.device(), &fenceInfo, nullptr, &fence);
	}


	BlackHoleApp::~BlackHoleApp() {
		vkDestroyFence(narwhalDevice.device(), fence, nullptr);
	}

	void BlackHoleApp::run()
	{
		//SETUP
		// create imgui, and pass in dependencies
		NarwhalImgui narwhalImgui{
			narwhalDevice,
			narwhalWindow,
			narwhalRenderer.getSwapChainRenderPass(),
			narwhalRenderer.getImageCount() };

		

		// Make Buffers
		std::vector<std::unique_ptr<NarwhalBuffer>> parameterBuffers(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		std::vector<std::unique_ptr<NarwhalBuffer>> uboBuffers(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		// Make Storage Images
		std::vector<std::unique_ptr<NarwhalStorageImage>> storageColorImages(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		std::vector<std::unique_ptr<NarwhalStorageImage>> storagePositionImages(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		std::vector<std::unique_ptr<NarwhalStorageImage>> storageDirectionImages(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkExtent2D swapChainExtent = narwhalWindow.getExtent();

		for (int i = 0; i < NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {

			parameterBuffers[i] = std::make_unique<NarwhalBuffer>(narwhalDevice, sizeof(BlackHoleComputeData),1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			uboBuffers[i] = std::make_unique<NarwhalBuffer>(narwhalDevice, sizeof(GlobalUbo),1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			storageColorImages[i] = std::make_unique<NarwhalStorageImage>(narwhalDevice, swapChainExtent.width, swapChainExtent.height);
			storagePositionImages[i] = std::make_unique<NarwhalStorageImage>(narwhalDevice, swapChainExtent.width, swapChainExtent.height);
			storageDirectionImages[i] = std::make_unique<NarwhalStorageImage>(narwhalDevice, swapChainExtent.width, swapChainExtent.height);

			parameterBuffers[i]->map();
			uboBuffers[i]->map();
			
		}

		auto computeSetLayout = NarwhalDescriptorSetLayout::Builder(narwhalDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT) // Parameters
			.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // Color Image
			.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // Position Image
			.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // Direction Image
			.build();

		auto renderSetLayout = NarwhalDescriptorSetLayout::Builder(narwhalDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS) // Global UBO
			.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT) // Color Image
			.build();


		//Make Descriptor Sets
		std::vector<VkDescriptorSet> computeDescriptorSets(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		std::vector<VkDescriptorSet> renderDescriptorSets(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < computeDescriptorSets.size(); i++) {
			auto paramBufferInfo = parameterBuffers[i]->descriptorInfo();
			auto colorImageInfo = storageColorImages[i]->getDescriptorImageInfo();
			auto positionImageInfo = storagePositionImages[i]->getDescriptorImageInfo();
			auto directionImageInfo = storageDirectionImages[i]->getDescriptorImageInfo();

			NarwhalDescriptorWriter(*computeSetLayout, *globalPool)
				.writeBuffer(0, &paramBufferInfo)
				.writeImage(1, &colorImageInfo)
				.writeImage(2, &positionImageInfo)
				.writeImage(3, &directionImageInfo)
				.build(computeDescriptorSets[i]);
		}

		for (int i = 0;  i < renderDescriptorSets.size();i++){
			auto uboBufferInfo = uboBuffers[i]->descriptorInfo();
			auto colorImageInfo = storageColorImages[i]->getDescriptorImageInfo();

			NarwhalDescriptorWriter(*renderSetLayout, *globalPool)
				.writeBuffer(0, &uboBufferInfo)
				.writeImage(1, &colorImageInfo)
				.build(renderDescriptorSets[i]);		
		};

		//Load Systems

		//Load Camera
		NarwhalCamera camera{};

		auto currentTime= std::chrono::high_resolution_clock::now();

		// Main Loop
		while (!narwhalWindow.shouldClose()) {
			glfwPollEvents(); 
			//Update
			auto newTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;
			deltaTime = glm::min(deltaTime, MAX_DT);
			
			

			

			if (auto commandBuffer = narwhalRenderer.beginFrame()) { //Will return a null ptr if swap chain needs to be recreated


				
				narwhalRenderer.beginSwapChainRenderPass(commandBuffer);

				renderImgui(narwhalImgui, commandBuffer);
				narwhalRenderer.endSwapChainRenderPass(commandBuffer);
				narwhalRenderer.endFrame();
			}
			


		}
		vkDeviceWaitIdle(narwhalDevice.device());

	}

	void BlackHoleApp::renderImgui(NarwhalImgui& narwhalImgui, VkCommandBuffer commandBuffer)
	{
		if (!showImgui) return;

		narwhalImgui.newFrame();
		// Start the Dear ImGui frame

		static int blackHoleType = 0;

		ImGui::Begin("Black Hole Parameters");
		
		//Blackhole type selection
		ImGui::Text("Black Hole Type"); ImGui::SameLine();
		ImGui::RadioButton("Schwarzschild", &blackHoleType, 0); ImGui::SameLine();
		ImGui::RadioButton("Kerr", &blackHoleType, 1);

		blackHoleParameters.blackHoleType = (BlackHoleType) blackHoleType;

		if (ImGui::CollapsingHeader("Step Size Parameters")) {
			ImGui::SliderFloat("Time Step", &blackHoleParameters.timeStep, 0.0f, 1.0f);
			ImGui::SliderFloat("Pole Margin", &blackHoleParameters.poleMargin, 0.f, 1.f,"%.4f");
			ImGui::SliderFloat("Pole Step", &blackHoleParameters.poleStep, 0.f, 1.f,"%.5f");
			ImGui::SliderFloat("Escape Distance", &blackHoleParameters.escapeDistance, 100, 1000000,"%.1f");
		}
		if (ImGui::CollapsingHeader("Physical Parameters")) {
			ImGui::SliderFloat("Horizon Radius", &blackHoleParameters.horizonRadius, 0.1f, 2.f, "%.3f");

			if (BlackHoleType(blackHoleType) == BlackHoleType::Schwarzchild) ImGui::BeginDisabled();
			ImGui::SliderFloat("Spin Factor", &blackHoleParameters.spinFactor, -1.f, 1.f, "%.3f");
			if (BlackHoleType(blackHoleType) == BlackHoleType::Schwarzchild) ImGui::EndDisabled();

			ImGui::SliderFloat("Disk Max", &blackHoleParameters.diskMax, .1f, 10.f);
			ImGui::SliderFloat("Disk Temp", &blackHoleParameters.diskTemp, 1E3F, 1E4F);

			if (BlackHoleType(blackHoleType) == BlackHoleType::Schwarzchild) ImGui::BeginDisabled();
			ImGui::SliderFloat("Inner Fallof Rate", &blackHoleParameters.innerFalloffRate, .1f, 10.f, "%.3f");
			ImGui::SliderFloat("Outer Fallof Rate", &blackHoleParameters.outerFalloffRate, .1f, 10.f, "%.3f");
			if (BlackHoleType(blackHoleType) == BlackHoleType::Schwarzchild) ImGui::EndDisabled();

			ImGui::SliderFloat("Beam Exponent", &blackHoleParameters.beamExponent, .1f, 10.f, "%.3f");
			ImGui::SliderFloat("Rotation Speed", &blackHoleParameters.rotationSpeed, .01f, 2.f);
			ImGui::SliderFloat("Time Delay Factor", &blackHoleParameters.timeDelayFactor, .01f, 1.f);

			if (BlackHoleType(blackHoleType) == BlackHoleType::Schwarzchild) ImGui::BeginDisabled();
			ImGui::Checkbox("Viscious Disk", &blackHoleParameters.visciousDisk);
			ImGui::Checkbox("Relative Temp", &blackHoleParameters.relativeTemp);
			if (BlackHoleType(blackHoleType) == BlackHoleType::Schwarzchild) ImGui::EndDisabled();

		}

		if (ImGui::CollapsingHeader("Noise Parameters")) {
			ImGui::SliderFloat3("Noise Offset", &blackHoleParameters.noiseOffset.x, -1.f, 1.f);
			ImGui::SliderFloat("Noise Scale", &blackHoleParameters.noiseScale, 0.f, 1.f);
			ImGui::SliderFloat("Noise Circulation", &blackHoleParameters.noiseCirculation, .01f, 10.f);
			ImGui::SliderFloat("Noise H", &blackHoleParameters.noiseH, 0.f, 3.f);
			ImGui::SliderInt("Noise Octaves", &blackHoleParameters.noiseOctaves, 1, 10);
		};

		if (ImGui::CollapsingHeader("Volumetric Noise Parameters")) {
			ImGui::SliderFloat("Step Size", &blackHoleParameters.stepSize, 0.f, 1.f);
			ImGui::SliderFloat("Absorption Factor", &blackHoleParameters.absorptionFactor, 0.f, 1.f);
			ImGui::SliderFloat("Noise Cutoff", &blackHoleParameters.noiseCutoff, 0.f, 1.f);
			ImGui::SliderFloat("Noise Multiplier", &blackHoleParameters.noiseMultiplier, 0.f, 5.f);
			ImGui::SliderInt("Max Steps", &blackHoleParameters.maxSteps, 1, 1000);
		}

		if (ImGui::CollapsingHeader("Brightness Parameters")) {
			ImGui::SliderFloat("Disk Multiplier", &blackHoleParameters.diskMultiplier, 0.f, 3.f);
			ImGui::SliderFloat("Stars Multiplier", &blackHoleParameters.starMultiplier, 0.f, 3.f);
		}
		

		ImGui::End();

		narwhalImgui.render(commandBuffer);
	}
	

	


}