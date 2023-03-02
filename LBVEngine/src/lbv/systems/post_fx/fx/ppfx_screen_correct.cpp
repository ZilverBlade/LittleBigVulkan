#pragma once
#include <lbv/dll.h>
#include "ppfx_screen_correct.h"
#include <lbv/vkapi/swap_chain.h>

namespace LittleBigVulkan::PostFX {
	struct ScreenCorrectionPush {
		float ditherIntensity;
		float invGamma;
	};

	ScreenCorrection::ScreenCorrection(
		LBVDevice& device, 
		glm::vec2 size,
		LBVDescriptorPool& descriptorPool,
		LBVFramebufferAttachment* inputAttachment, 
		LBVSwapChain* swapChain
	) : PostFX::Effect(device), lbvDescriptorPool(descriptorPool), inputAttachment(inputAttachment) {
		
		ppfxSceneDescriptorLayout = LBVDescriptorSetLayout::Builder(lbvDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();
		createSceneDescriptors();
		createPipelineLayout();
		createPipeline(LBVShader(LBVShaderType::Fragment, "res/shaders/spirv/screen_correct.fsh.spv"), swapChain->getRenderPass());
	}
	ScreenCorrection::~ScreenCorrection() {

	}
	void ScreenCorrection::render(FrameInfo& frameInfo) {
		ScreenCorrectionPush push{};
		push.invGamma = 1.0f / 2.2f;
		push.ditherIntensity = 0.5f / 256.f; // 8 bit
		//push.ditherIntensity = 0.5f / 1024.f; // 10 bit

		ppfxPipeline->bind(frameInfo.commandBuffer);
		ppfxPush.push(frameInfo.commandBuffer, ppfxPipelineLayout->getPipelineLayout(), &push);
		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			ppfxPipelineLayout->getPipelineLayout(),
			0,
			1,
			&ppfxSceneDescriptorSet,
			0,
			nullptr
		);
		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}
	void ScreenCorrection::resize(glm::vec2 size, const std::vector<LBVFramebufferAttachment*>& newInputAttachments) {
		inputAttachment = newInputAttachments[0]; // only 1 scene input attachment
		createSceneDescriptors();
	}
	void ScreenCorrection::createPipelineLayout() {
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
			ppfxSceneDescriptorLayout->getDescriptorSetLayout()
		};
		ppfxPush = LBVPushConstant(128, VK_SHADER_STAGE_FRAGMENT_BIT);
		std::vector<VkPushConstantRange> pushConstantRanges{
			ppfxPush.getRange()
		};
		ppfxPipelineLayout = std::make_unique<LBVPipelineLayout>(lbvDevice, pushConstantRanges, descriptorSetLayouts);
	}
	void ScreenCorrection::createPipeline(const LBVShader& fragmentShader, VkRenderPass renderPass) {
		LBVGraphicsPipelineConfigInfo pipelineConfig{};
		pipelineConfig.setSampleCount(VK_SAMPLE_COUNT_1_BIT);
		//pipelineConfig.setCullMode(VK_CULL_MODE_BACK_BIT);
		pipelineConfig.disableDepthTest();

		pipelineConfig.pipelineLayout = ppfxPipelineLayout->getPipelineLayout();
		pipelineConfig.renderPass = renderPass;
		ppfxPipeline = std::make_unique<LBVGraphicsPipeline>(
			lbvDevice, std::vector<LBVShader>{
			LBVShader{ LBVShaderType::Vertex, "res/shaders/spirv/full_screen.vsh.spv" },
				fragmentShader
		},
			pipelineConfig
		);
	}
	void ScreenCorrection::createSceneDescriptors() {
		VkDescriptorImageInfo imageInfo = inputAttachment->getDescriptor();
		LBVDescriptorWriter(*ppfxSceneDescriptorLayout, lbvDescriptorPool)
			.writeImage(0, &imageInfo)
			.build(ppfxSceneDescriptorSet);
	}
}