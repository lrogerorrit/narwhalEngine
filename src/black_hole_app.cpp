#include "black_hole_app.hpp"

#include "narwhal_camera.hpp"
#include "narwhal_buffer.hpp"
#include "narwhal_storage_image.hpp"
#include "narwhal_cubemap.hpp"
#include "narwhal_model.hpp"
#include "narwhal_image.hpp"
#include "narwhal_cameraV2.hpp"

#include "systems/narwhal_imgui.hpp"
#include "systems/black_hole_compute_system.hpp"
#include "systems/quad_render_system.hpp"
#include "systems/black_hole_init_system.hpp"



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
#include <string>


#define MAX_DT 1.f //TODO: Change and tune


namespace narwhal {
	//For more alignment info check here:	https://registry.khronos.org/vulkan/specs/1.0-wsi_extensions/html/vkspec.html#interfaces-resources-layout
	//										https://www.oreilly.com/library/view/opengl-programming-guide/9780132748445/app09lev1sec2.html

	struct Vertex {
		glm::vec3 position;
		glm::vec2 uv;
	};

	BlackHoleApp::BlackHoleApp() {
		const int POOL_SETS_COUNT = 9;
		globalPool = NarwhalDescriptorPool::Builder(narwhalDevice)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT*2)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT*8)
			.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT*2)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT)
			.setMaxSets(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT * POOL_SETS_COUNT)
			.build();

		//Load Fence
		VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		vkCreateFence(narwhalDevice.device(), &fenceInfo, nullptr, &fence);
	}


	BlackHoleApp::~BlackHoleApp() {
		vkDestroyFence(narwhalDevice.device(), fence, nullptr);
	}

	void BlackHoleApp::updateComputeDescriptors(int frameNum, NarwhalDescriptorSetLayout& setLayout) {

	}
	void BlackHoleApp::updateRenderDescriptors(int frameNum, NarwhalDescriptorSetLayout& setLayout) {

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

		
		VkExtent2D swapChainExtent = narwhalWindow.getExtent();

		// Make Buffers
		std::vector<std::unique_ptr<NarwhalBuffer>> parameterBuffers(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		std::vector<std::unique_ptr<NarwhalBuffer>> uboBuffers(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		// Make Storage Images
		NarwhalStorageImage storageColorImage(narwhalDevice, swapChainExtent.width, swapChainExtent.height);
		NarwhalStorageImage storagePositionImage(narwhalDevice, swapChainExtent.width, swapChainExtent.height);
		NarwhalStorageImage storageDirectionImage(narwhalDevice, swapChainExtent.width, swapChainExtent.height);
		NarwhalStorageImage storageCompleteImage(narwhalDevice, swapChainExtent.width, swapChainExtent.height, VK_FORMAT_R8_UINT);

		//Make init data
		std::unique_ptr<NarwhalBuffer> frameInitBuffer= std::make_unique<NarwhalBuffer>(narwhalDevice,sizeof(InitParameters), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		frameInitBuffer->map();

		//Make Cubemap Images
		std::string rightPath = "data/textures/cubemap/right.png";
		std::string leftPath = "data/textures/cubemap/left.png";
		std::string topPath = "data/textures/cubemap/top.png";
		std::string bottomPath = "data/textures/cubemap/bottom.png";
		std::string frontPath = "data/textures/cubemap/front.png";
		std::string backPath = "data/textures/cubemap/back.png";
		NarwhalImage tempImage(narwhalDevice, "data/textures/blackbody.png");
		NarwhalCubemap cubemapImage(narwhalDevice, 1024, 1024, rightPath, leftPath, topPath, bottomPath, frontPath, backPath);
		//Make other Images


		for (int i = 0; i < NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {

			parameterBuffers[i] = std::make_unique<NarwhalBuffer>(narwhalDevice, sizeof(BlackHoleComputeData),1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			uboBuffers[i] = std::make_unique<NarwhalBuffer>(narwhalDevice, sizeof(GlobalUbo),1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			parameterBuffers[i]->map();
			uboBuffers[i]->map();
			
		}

		auto initSetLayout = NarwhalDescriptorSetLayout::Builder(narwhalDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT) // Parameters
			.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // Color Image
			.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // Position Image
			.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // Direction Image
			.addBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // isComplete Image
			.build();
		
		auto computeSetLayout = NarwhalDescriptorSetLayout::Builder(narwhalDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT) // Parameters
			.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // Color Image
			.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // Position Image
			.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // Direction Image
			.addBinding(4, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // Temp Image
			.addBinding(5, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT) // isComplete Image
			.addBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT) // Background Cube map Image
			.build();

		

		auto renderSetLayout = NarwhalDescriptorSetLayout::Builder(narwhalDevice)
			//.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS) // Global UBO
			.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT) // Color Image
			.build();


		//Make Descriptor Sets
		std::vector<VkDescriptorSet> computeDescriptorSets(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		std::vector<VkDescriptorSet> renderDescriptorSets(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		std::vector<VkDescriptorSet> initDescriptorSets(NarwhalSwapChain::MAX_FRAMES_IN_FLIGHT);
		VkDescriptorSet initDescriptorSet;

		{
			auto initBufferInfo= frameInitBuffer->descriptorInfo();
			auto colorImageInfo = storageColorImage.getDescriptorImageInfo();
			auto positionImageInfo = storagePositionImage.getDescriptorImageInfo();
			auto directionImageInfo = storageDirectionImage.getDescriptorImageInfo();
			auto completeImageInfo = storageCompleteImage.getDescriptorImageInfo();

			NarwhalDescriptorWriter(*initSetLayout, *globalPool)
				.writeBuffer(0, &initBufferInfo)
				.writeImage(1, &colorImageInfo)
				.writeImage(2, &positionImageInfo)
				.writeImage(3, &directionImageInfo)
				.writeImage(4, &completeImageInfo)
				.build(initDescriptorSet);
		}

		for (int i = 0; i < computeDescriptorSets.size(); i++) {
			auto paramBufferInfo = parameterBuffers[i]->descriptorInfo();
			auto colorImageInfo = storageColorImage.getDescriptorImageInfo();
			auto positionImageInfo = storagePositionImage.getDescriptorImageInfo();
			auto directionImageInfo = storageDirectionImage.getDescriptorImageInfo();
			auto backgroundCubeMapInfo = cubemapImage.getDescriptorImageInfo();
			auto completeImageInfo = storageCompleteImage.getDescriptorImageInfo();
			auto tempImageInfo = tempImage.getDescriptorImageInfo();


			NarwhalDescriptorWriter(*computeSetLayout, *globalPool)
				.writeBuffer(0, &paramBufferInfo)
				.writeImage(1, &colorImageInfo)
				.writeImage(2, &positionImageInfo)
				.writeImage(3, &directionImageInfo)
				.writeImage(4, &tempImageInfo)
				.writeImage(5,&completeImageInfo)
				.writeImage(6, &backgroundCubeMapInfo)
				.build(computeDescriptorSets[i]);
		}

		for (int i = 0;  i < renderDescriptorSets.size();i++){
			auto uboBufferInfo = uboBuffers[i]->descriptorInfo();
			auto colorImageInfo = storageColorImage.getDescriptorImageInfo();

			NarwhalDescriptorWriter(*renderSetLayout, *globalPool)
				//.writeBuffer(0, &uboBufferInfo)
				.writeImage(0, &colorImageInfo)
				.build(renderDescriptorSets[i]);		
		};

		



		//Load Systems
		BlackHoleComputeSystem blackHoleComputeSystem{ narwhalDevice, narwhalRenderer.getSwapChainRenderPass(), computeSetLayout->getDescriptorSetLayout()};
		QuadRenderSystem quadRenderSystem{ narwhalDevice, narwhalRenderer.getSwapChainRenderPass(), renderSetLayout->getDescriptorSetLayout()};
		BlackHoleInitSystem blackHoleInitSystem{ narwhalDevice, narwhalRenderer.getSwapChainRenderPass(), initSetLayout->getDescriptorSetLayout()};

		//Load Camera (TODO: REMOVE)
		NarwhalCamera camera{};

		auto viewerObject = NarwhalGameObject::createGameObject();
		viewerObject.transform.translation = glm::vec3(-4, 1, 0);
		viewerObject.transform.rotation = glm::vec3(glm::radians(15.f), glm::radians(90.f), glm::radians(15.0f));

		camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

		//Load Camera V2
		VkExtent2D newSize = narwhalWindow.getExtent();
		const glm::vec3 cameraPos = glm::vec3(-4,1,0);
		const glm::vec3 cameraTarget = glm::vec3(0,0,0);
		NarwhalCameraV2 cameraV2;
		cameraV2.setPerspective( 90, newSize.width / (float)newSize.height, 0.3f, 1000.0f);
		cameraV2.lookAt( cameraPos, cameraTarget);
		cameraV2.enable();
		auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = startTime;
		VkExtent2D oldSize = narwhalWindow.getExtent();



		bool shouldInitFrame = true;

		// Main Loop
		while (!narwhalWindow.shouldClose()) {
			glfwPollEvents(); 
			//Update
			auto newTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;
			deltaTime = glm::min(deltaTime, MAX_DT);
			
			//Update size (TODO)
			newSize = narwhalWindow.getExtent();
			if (newSize.width != oldSize.width || newSize.height != oldSize.height) {
				oldSize = newSize;
				//TODO: Change size of images
			}


			

			if (auto commandBuffer = narwhalRenderer.beginFrame()) { //Will return a null ptr if swap chain needs to be recreated
				int frameIndex = narwhalRenderer.getFrameIndex();
				
				if (shouldInitFrame) {
					//shouldInitFrame = false;
					/*
					glm::mat4 camToWorld = glm::mat4(0.06699, 0.25000, -0.96593, -4.00000, 0.25000, 0.93301, 0.25882, 1.00000, -0.96593, 0.25882, 0.00000, 0.00000, 0.00000, 0.00000, 0.00000, 1.00000);
					glm::mat4 invProj = glm::mat4(2.12548, 0.00000, 0.00000, 0.00000,0.00000, 1.00000, 0.00000, 0.00000,0.00000, 0.00000, 0.00000, -1.00000,0.00000, 0.00000, -1.66617, 1.66717);
					glm::vec3 cart= glm::vec3(-4.00000, 1.00000, 0.00000);
					glm::vec3 sph = glm::vec3(4.12, 1.33, 3.14);
					float horizon = .5;

					initParameters.windowSize = glm::ivec2(newSize.width, newSize.height);
					initParameters.camToWorld = camToWorld;
					initParameters.camInverseProj = invProj;
					initParameters.camPosCartesian = cart;
					initParameters.camPosSpherical = sph;
					initParameters.horizonRadius = horizon;
					
					float r = glm::length(camera.getPosition());
					float theta= atan2(glm::length(glm::vec2(camera.getPosition().x,camera.getPosition().z)),camera.getPosition().y);
					float phi = atan2(camera.getPosition().z, camera.getPosition().x);

					initParameters.windowSize= glm::ivec2(newSize.width, newSize.height);
					initParameters.camToWorld= glm::inverse(camera.getView());
					initParameters.camInverseProj = glm::inverse(camera.getProjection());
					initParameters.camPosCartesian = camera.getPosition();
					initParameters.camPosSpherical = glm::vec3(r,theta,phi);
					initParameters.horizonRadius = computeData.params.horizonRadius;
					*/

					float r = glm::length(cameraV2.eye);
					float theta = atan2(glm::length(glm::vec2(cameraV2.eye.x, cameraV2.eye.z)), cameraV2.eye.y);
					float phi = atan2(cameraV2.eye.z, cameraV2.eye.x);
					
					Matrix44 invView = cameraV2.view_matrix;
					Matrix44 invProj = cameraV2.projection_matrix;
					Matrix44 invViewProj = cameraV2.viewprojection_matrix;
					invView.inverse();
					invProj.inverse();
					invViewProj.inverse();
					
					Matrix44 camToWorld = invView * invProj;

					initParameters.windowSize= glm::ivec2(newSize.width, newSize.height);
					initParameters.camToWorld = cameraV2.view_matrix;
					initParameters.camInverseProj = invProj;
					initParameters.camPosCartesian = cameraV2.eye;
					initParameters.camPosSpherical = glm::vec3(r,theta,phi);
					initParameters.horizonRadius = computeData.params.horizonRadius;
					


					frameInitBuffer->writeToBuffer(&initParameters, sizeof(initParameters));

					auto initBufferInfo = frameInitBuffer->descriptorInfo();
					auto colorImageInfo = storageColorImage.getDescriptorImageInfo();
					auto positionImageInfo = storagePositionImage.getDescriptorImageInfo();
					auto directionImageInfo = storageDirectionImage.getDescriptorImageInfo();
					auto completeImageInfo = storageCompleteImage.getDescriptorImageInfo();

					NarwhalDescriptorWriter(*initSetLayout, *globalPool)
						.writeBuffer(0, &initBufferInfo)
						.writeImage(1, &colorImageInfo)
						.writeImage(2, &positionImageInfo)
						.writeImage(3, &directionImageInfo)
						.writeImage(4, &completeImageInfo)
						.overwrite(initDescriptorSet);


					InitFrameInfo initFrameInfo{frameIndex,commandBuffer,initDescriptorSet,fence};
					blackHoleInitSystem.initFrame(initFrameInfo, newSize);
				}
				
				computeData.windowSize = glm::ivec2(newSize.width, newSize.height);
				computeData.time = std::chrono::duration<float, std::chrono::seconds::period>(newTime - startTime).count();
				//Update compute descriptor sets
				
				//Update computeDescriptorSets 0 with blackHoleParameters
				parameterBuffers[frameIndex]->writeToBuffer(&computeData, sizeof(BlackHoleComputeData));
				
				auto paramBufferInfo = parameterBuffers[frameIndex]->descriptorInfo();
				auto colorImageInfo = storageColorImage.getDescriptorImageInfo();
				auto positionImageInfo = storagePositionImage.getDescriptorImageInfo();
				auto directionImageInfo = storageDirectionImage.getDescriptorImageInfo();
				auto backgroundCubeMapInfo = cubemapImage.getDescriptorImageInfo();
				auto completeImageInfo = storageCompleteImage.getDescriptorImageInfo();
				auto tempImageInfo = tempImage.getDescriptorImageInfo();


				NarwhalDescriptorWriter(*computeSetLayout, *globalPool)
					.writeBuffer(0, &paramBufferInfo)
					.writeImage(1, &colorImageInfo)
					.writeImage(2, &positionImageInfo)
					.writeImage(3, &directionImageInfo)
					.writeImage(4, &tempImageInfo)
					.writeImage(5, &completeImageInfo)
					.writeImage(6, &backgroundCubeMapInfo)
					.overwrite(computeDescriptorSets[frameIndex]);


				BlackHoleFrameInfo frameInfo{frameIndex,deltaTime,commandBuffer,computeDescriptorSets[frameIndex],fence };

				//blackHoleComputeSystem.render(frameInfo, blackHoleParameters, newSize);

				//Update Render Descriptor Sets
				NarwhalDescriptorWriter(*renderSetLayout, *globalPool)
					//.writeImage(0, &colorImageInfo)
					.writeImage(0, &directionImageInfo)
					.overwrite(renderDescriptorSets[frameIndex]);

				QuadFrameInfo quadFrameInfo{ frameIndex,commandBuffer,renderDescriptorSets[frameIndex] };
				

				narwhalRenderer.beginSwapChainRenderPass(commandBuffer);
				quadRenderSystem.render(quadFrameInfo);	
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

		if (NarwhalCameraV2::current) {
			NarwhalCameraV2::current->renderInMenu();
		}
		
		//Blackhole type selection
		ImGui::Text("Black Hole Type"); ImGui::SameLine();
		ImGui::RadioButton("Schwarzschild", &blackHoleType, 0); ImGui::SameLine();
		ImGui::RadioButton("Kerr", &blackHoleType, 1);
		
		computeData.params.blackHoleType = (BlackHoleType) blackHoleType;

		if (ImGui::CollapsingHeader("Step Size Parameters")) {
			ImGui::SliderFloat("Time Step", &computeData.params.timeStep, 0.0f, 1.0f);
			ImGui::SliderFloat("Pole Margin", &computeData.params.poleMargin, 0.f, 1.f,"%.4f");
			ImGui::SliderFloat("Pole Step", &computeData.params.poleStep, 0.f, 1.f,"%.5f");
			ImGui::SliderFloat("Escape Distance", &computeData.params.escapeDistance, 100, 1000000,"%.1f");
		}
		if (ImGui::CollapsingHeader("Physical Parameters")) {
			ImGui::SliderFloat("Horizon Radius", &computeData.params.horizonRadius, 0.1f, 2.f, "%.3f");

			if (BlackHoleType(blackHoleType) == BlackHoleType::Schwarzchild) ImGui::BeginDisabled();
			ImGui::SliderFloat("Spin Factor", &computeData.params.spinFactor, -1.f, 1.f, "%.3f");
			if (BlackHoleType(blackHoleType) == BlackHoleType::Schwarzchild) ImGui::EndDisabled();

			ImGui::SliderFloat("Disk Max", &computeData.params.diskMax, .1f, 10.f);
			ImGui::SliderFloat("Disk Temp", &computeData.params.diskTemp, 1E3F, 1E4F);

			if (BlackHoleType(blackHoleType) == BlackHoleType::Schwarzchild) ImGui::BeginDisabled();
			ImGui::SliderFloat("Inner Fallof Rate", &computeData.params.innerFalloffRate, .1f, 10.f, "%.3f");
			ImGui::SliderFloat("Outer Fallof Rate", &computeData.params.outerFalloffRate, .1f, 10.f, "%.3f");
			if (BlackHoleType(blackHoleType) == BlackHoleType::Schwarzchild) ImGui::EndDisabled();

			ImGui::SliderFloat("Beam Exponent", &computeData.params.beamExponent, .1f, 10.f, "%.3f");
			ImGui::SliderFloat("Rotation Speed", &computeData.params.rotationSpeed, .01f, 2.f);
			ImGui::SliderFloat("Time Delay Factor", &computeData.params.timeDelayFactor, .01f, 1.f);

			if (BlackHoleType(blackHoleType) == BlackHoleType::Schwarzchild) ImGui::BeginDisabled();
			ImGui::Checkbox("Viscious Disk", &computeData.params.visciousDisk);
			ImGui::Checkbox("Relative Temp", &computeData.params.relativeTemp);
			if (BlackHoleType(blackHoleType) == BlackHoleType::Schwarzchild) ImGui::EndDisabled();

		}

		if (ImGui::CollapsingHeader("Noise Parameters")) {
			ImGui::SliderFloat3("Noise Offset", &computeData.params.noiseOffset.x, -1.f, 1.f);
			ImGui::SliderFloat("Noise Scale", &computeData.params.noiseScale, 0.f, 1.f);
			ImGui::SliderFloat("Noise Circulation", &computeData.params.noiseCirculation, .01f, 10.f);
			ImGui::SliderFloat("Noise H", &computeData.params.noiseH, 0.f, 3.f);
			ImGui::SliderInt("Noise Octaves", &computeData.params.noiseOctaves, 1, 10);
		};

		if (ImGui::CollapsingHeader("Volumetric Noise Parameters")) {
			ImGui::SliderFloat("Step Size", &computeData.params.stepSize, 0.f, 1.f);
			ImGui::SliderFloat("Absorption Factor", &computeData.params.absorptionFactor, 0.f, 1.f);
			ImGui::SliderFloat("Noise Cutoff", &computeData.params.noiseCutoff, 0.f, 1.f);
			ImGui::SliderFloat("Noise Multiplier", &computeData.params.noiseMultiplier, 0.f, 5.f);
			ImGui::SliderInt("Max Steps", &computeData.params.maxSteps, 1, 1000);
		}

		if (ImGui::CollapsingHeader("Brightness Parameters")) {
			ImGui::SliderFloat("Disk Multiplier", &computeData.params.diskMultiplier, 0.f, 3.f);
			ImGui::SliderFloat("Stars Multiplier", &computeData.params.starMultiplier, 0.f, 3.f);
		}
		

		ImGui::End();

		narwhalImgui.render(commandBuffer);
	}
	

	


}