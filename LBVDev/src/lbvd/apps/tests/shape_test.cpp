#include "shape_test.h"

#include <chrono>
#include <lbv/rendering/camera.h>
#include <lbv/systems/resource_system.h>
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
		
		Actor cameraActor = level->createActor("camera actor");
		
		LBVFramebufferAttachmentCreateInfo depthAttachmentCreateInfo{};
		depthAttachmentCreateInfo.dimensions = glm::ivec3(resolution, 1);
		depthAttachmentCreateInfo.framebufferFormat = VK_FORMAT_D32_SFLOAT;
		depthAttachmentCreateInfo.framebufferType = LBVFramebufferAttachmentType::Depth;
		depthAttachmentCreateInfo.imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
		depthAttachmentCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depthAttachmentCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		depthAttachmentCreateInfo.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachmentCreateInfo.sampleCount = VK_SAMPLE_COUNT_1_BIT;
		depthAttachmentCreateInfo.linearFiltering = false;
		LBVFramebufferAttachment* depthAttachment = new LBVFramebufferAttachment(lbvDevice, depthAttachmentCreateInfo);

		LBVFramebufferAttachmentCreateInfo colorAttachmentCreateInfo{};
		colorAttachmentCreateInfo.dimensions = glm::ivec3(resolution, 1);
		colorAttachmentCreateInfo.framebufferFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
		colorAttachmentCreateInfo.framebufferType = LBVFramebufferAttachmentType::Color;
		colorAttachmentCreateInfo.imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
		colorAttachmentCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorAttachmentCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		colorAttachmentCreateInfo.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		colorAttachmentCreateInfo.sampleCount = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentCreateInfo.linearFiltering = false;
		LBVFramebufferAttachment* colorAttachment = new LBVFramebufferAttachment(lbvDevice, colorAttachmentCreateInfo);

		LBVAttachmentInfo depthAttachmentInfo{};
		depthAttachmentInfo.framebufferAttachment = depthAttachment;
		depthAttachmentInfo.clear.depth = { 1.0, 0 };
		depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		LBVAttachmentInfo colorAttachmentInfo{};
		colorAttachmentInfo.framebufferAttachment = colorAttachment;
		colorAttachmentInfo.clear.color = { 0.0, 0.0, 0.0, 1.0 };
		colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		LBVRenderPass* renderPass = new LBVRenderPass(lbvDevice, { depthAttachmentInfo, colorAttachmentInfo });
		LBVFramebuffer* framebuffer = new LBVFramebuffer(lbvDevice, renderPass, { depthAttachment, colorAttachment });

		LBVPushConstant push = LBVPushConstant(sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT);
		LBVPipelineLayout* pipelineLayout = new LBVPipelineLayout(lbvDevice, { push.getRange() }, { globalUBODescriptorLayout->getDescriptorSetLayout() });

		LBVGraphicsPipelineConfigInfo pipelineConfigInfo{};
		pipelineConfigInfo.enableShapeDescriptions();
		pipelineConfigInfo.setSampleCount(VK_SAMPLE_COUNT_1_BIT);
		//pipelineConfigInfo.setCullMode(VK_CULL_MODE_BACK_BIT);
		pipelineConfigInfo.renderPass = renderPass->getRenderPass();
		pipelineConfigInfo.pipelineLayout = pipelineLayout->getPipelineLayout();
		LBVGraphicsPipeline* renderPipeline = new LBVGraphicsPipeline(
			lbvDevice,
			{ LBVShader(LBVShaderType::Vertex, "res/shaders/spirv/shape.vsh.spv"), LBVShader(LBVShaderType::Fragment, "res/shaders/spirv/forward_shading.fsh.spv") },
			pipelineConfigInfo
		);

		PostFX::Effect* screenCorrection = new PostFX::ScreenCorrection(
			lbvDevice,
			resolution,
			*staticPool,
			colorAttachment,
			lbvSwapChain
		);

		LBVCamera camera{};
		camera.setPerspectiveProjection(50.0f, 1.0f, 0.1f, 32.f);
		cameraActor.getTransform().translation = { 0.0, -3.0, 0.0 };

		std::vector<std::unique_ptr<LBVCommandBuffer>> commandBuffers{};
		commandBuffers.resize(lbvSwapChain->getImageCount());
		for (auto& cb : commandBuffers) {
			cb = std::make_unique<LBVCommandBuffer>(lbvDevice);
		}

		auto shapeData = LBVShape::Builder()
			.addTriangle({ glm::vec2{-1.0, -1.0}, glm::vec2{-1.0, 1.0}, glm::vec2{1.0, 1.0} }) // not sure why the winding order screws up and generates back faces
			.addTriangle({ glm::vec2{1.0, 1.0}, glm::vec2{1.0, -1.0}, glm::vec2{-1.0, -1.0} })
			.setDepth(0.5f)
			.build();
		std::unique_ptr<LBVShape> square3D = std::make_unique<LBVShape>(lbvDevice, shapeData);

		Controllers::CameraController controller{};

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


			uint32_t frameIndex = lbvSwapChain->getImageIndex();
			if (VkResult result = lbvSwapChain->acquireNextImage(&frameIndex); result == VK_SUCCESS) {
				VkCommandBuffer commandBuffer = commandBuffers[frameIndex]->getCommandBuffer();
				commandBuffers[frameIndex]->begin();

				FrameInfo frameInfo{};
				frameInfo.level = level;
				frameInfo.frameTime = frameTime;
				frameInfo.frameIndex = frameIndex;
				frameInfo.globalUBO = renderData[frameIndex].uboDescriptorSet;
				frameInfo.sceneSSBO = renderData[frameIndex].ssboDescriptorSet;
				frameInfo.commandBuffer = commandBuffer;
				frameInfo.resourceSystem = nullptr;

				GlobalUBO ubo{};
				ubo.viewMatrix = camera.getView();
				ubo.invViewMatrix = camera.getInverseView();
				ubo.projMatrix = camera.getProjection();
				ubo.invProjMatrix = camera.getInverseProjection();
				ubo.viewProjMatrix = ubo.projMatrix * ubo.viewMatrix;

				renderData[frameIndex].uboBuffer->writeToBuffer(&ubo);
				renderData[frameIndex].uboBuffer->flush();

				renderPass->beginRenderPass(commandBuffer, framebuffer);
				renderPipeline->bind(frameInfo.commandBuffer);
				vkCmdBindDescriptorSets(
					frameInfo.commandBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					pipelineLayout->getPipelineLayout(),
					0,
					1,
					&frameInfo.globalUBO,
					0,
					nullptr
				);
				glm::mat4 identity(1.0f);
				push.push(commandBuffer, pipelineLayout->getPipelineLayout(), &identity);
				square3D->bind(commandBuffer);
				square3D->draw(commandBuffer);

				renderPass->endRenderPass(commandBuffer);


				lbvSwapChain->beginRenderPass(commandBuffer);
				screenCorrection->render(frameInfo);
				lbvSwapChain->endRenderPass(commandBuffer);

				commandBuffers[frameIndex]->end();

				lbvRenderer->submitCommandBuffers({ commandBuffers[frameIndex].get() });
				lbvRenderer->submitQueue(lbvSwapChain->getSubmitInfo(&frameIndex), lbvSwapChain->getFence(frameIndex));
				lbvSwapChain->present(lbvRenderer->getQueue(), &frameIndex);
			}
		}
		vkDeviceWaitIdle(lbvDevice.getDevice());
	}
}