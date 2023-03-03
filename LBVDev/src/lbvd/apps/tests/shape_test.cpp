#include "shape_test.h"

#include <chrono>
#include <lbv/rendering/camera.h>
#include <lbv/systems/resource_system.h>
#include <lbv/systems/rendering/forward_render_system.h>
#include <lbv/systems/rendering/light_system.h>
#include <lbv/systems/post_fx/fx/ppfx_screen_correct.h>
#include <lbv/controllers/camera_controller.h>
#include <lbv/graphics/surface_material.h>
#include <lbv/graphics/shape.h>

namespace LittleBigVulkan::Apps {
	ShapeTest::ShapeTest(const char* name) : VulkanBaseApp(name) {}
	ShapeTest::~ShapeTest() {}
	void ShapeTest::run() {
		level = std::make_shared<Level>();
		glm::vec2 resolution = { 1280, 720 };
		
		Actor cameraActor = level->createActor("some kind of camera actor");
		Actor skyActor = level->createActor("some kind of sky");
		Actor shapeActor = level->createActor("some kind of object");

		VkSampleCountFlagBits msaa = VK_SAMPLE_COUNT_4_BIT;

		RenderSystem* renderSystem = new ForwardRenderSystem(
			lbvDevice,
			resolution,
			{ globalUBODescriptorLayout->getDescriptorSetLayout(), sceneSSBODescriptorLayout->getDescriptorSetLayout() },
			msaa
		);
		LightSystem lightSystem{};

		PostFX::Effect* screenCorrection = new PostFX::ScreenCorrection(
			lbvDevice,
			resolution,
			*staticPool,
			renderSystem->getColorAttachment(),
			lbvSwapChain
		);

		LBVCamera camera{};
		camera.setPerspectiveProjection(50.0f, 1.0f, 0.1f, 32.f);
		cameraActor.getTransform().translation = { 0.0, -3.0, 0.0 };
		Controllers::CameraController controller{};

		skyActor.addComponent<SkyComponent>();
		skyActor.getTransform().rotation = { 0.7, 0.0, 2.2 };
		
		auto squareShape = LBVShape::Builder()
			.addTriangle({ glm::vec2{-1.0, -1.0}, glm::vec2{-1.0, 1.0}, glm::vec2{1.0, 1.0} }) // not sure why the winding order screws up and generates back faces
			.addTriangle({ glm::vec2{1.0, 1.0}, glm::vec2{1.0, -1.0}, glm::vec2{-1.0, -1.0} })
			.setDepth(0.5f)
			.build();
		std::shared_ptr<LBVShape> square3D = std::make_shared<LBVShape>(lbvDevice, squareShape);

		shapeActor.addComponent<ShapeComponent>();
		shapeActor.getComponent<ShapeComponent>().shape = std::move(square3D);


		std::vector<std::unique_ptr<LBVCommandBuffer>> commandBuffers{};
		commandBuffers.resize(lbvSwapChain->getImageCount());
		for (auto& cb : commandBuffers) {
			cb = std::make_unique<LBVCommandBuffer>(lbvDevice);
		}

		auto oldTime = std::chrono::high_resolution_clock::now();
		while (!lbvWindow.shouldClose()) {
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - oldTime).count();
			oldTime = newTime;
			glfwPollEvents();

			controller.moveTranslation(lbvWindow, frameTime, cameraActor);
			controller.moveOrientation(lbvWindow, frameTime, cameraActor);

			camera.setViewYXZ(cameraActor.getTransform().translation, cameraActor.getTransform().rotation);
			camera.setPerspectiveProjection(70.0f, lbvSwapChain->extentAspectRatio(), 0.01f, 128.f);


			uint32_t imageIndex = lbvSwapChain->getImageIndex();
			if (VkResult result = lbvSwapChain->acquireNextImage(&imageIndex); result == VK_SUCCESS) {
				VkCommandBuffer commandBuffer = commandBuffers[imageIndex]->getCommandBuffer();
				commandBuffers[imageIndex]->begin();

				FrameInfo frameInfo{};
				frameInfo.level = level;
				frameInfo.frameTime = frameTime;
				frameInfo.frameIndex = imageIndex;
				frameInfo.globalUBO = renderData[imageIndex].uboDescriptorSet;
				frameInfo.sceneSSBO = renderData[imageIndex].ssboDescriptorSet;
				frameInfo.commandBuffer = commandBuffer;
				frameInfo.resourceSystem = nullptr;

				GlobalUBO ubo{};
				ubo.viewMatrix = camera.getView();
				ubo.invViewMatrix = camera.getInverseView();
				ubo.projMatrix = camera.getProjection();
				ubo.invProjMatrix = camera.getInverseProjection();
				ubo.viewProjMatrix = ubo.projMatrix * ubo.viewMatrix;

				renderData[imageIndex].uboBuffer->writeToBuffer(&ubo);
				renderData[imageIndex].uboBuffer->flush();

				lightSystem.update(frameInfo, *renderData[imageIndex].sceneSSBO);
				renderData[imageIndex].ssboBuffer->writeToBuffer(renderData[imageIndex].sceneSSBO.get());
				renderData[imageIndex].ssboBuffer->flush();


				renderSystem->renderEarlyDepth(frameInfo);
				renderSystem->beginOpaquePass(frameInfo);
				renderSystem->renderOpaque(frameInfo);
				renderSystem->endOpaquePass(frameInfo);

				lbvSwapChain->beginRenderPass(commandBuffer);
				screenCorrection->render(frameInfo);
				lbvSwapChain->endRenderPass(commandBuffer);

				commandBuffers[imageIndex]->end();

				lbvRenderer->submitCommandBuffers({ commandBuffers[imageIndex].get() });
				lbvRenderer->submitQueue(lbvSwapChain->getSubmitInfo(&imageIndex), lbvSwapChain->getFence(imageIndex));
				lbvSwapChain->present(lbvRenderer->getQueue(), &imageIndex);
			}
		}
		vkDeviceWaitIdle(lbvDevice.getDevice());
	}
}