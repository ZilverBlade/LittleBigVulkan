#include "post_processing_fx.h"
#include <lbv/vkapi/framebuffer.h>
#include <lbv/vkapi/render_pass.h>

namespace LittleBigVulkan {
	LBVPostProcessingEffect::LBVPostProcessingEffect(
		LBVDevice& device,
		glm::vec2 resolution,
		const LBVShader& fragmentShader,
		LBVDescriptorPool& descriptorPool,
		const std::vector<VkDescriptorImageInfo>& inputAttachments,
		VkFormat framebufferFormat,
		VkImageViewType viewType,
		uint32_t layers,
		uint32_t mipLevels
	) : lbvDevice(device), inputAttachments(inputAttachments), ppfxFramebufferFormat(framebufferFormat), ppfxFramebufferViewType(viewType), descriptorPool(descriptorPool), layerCount(layers), mipLevels(mipLevels) {
		if (inputAttachments.size() != 0) {
			LBVDescriptorSetLayout::Builder builder = LBVDescriptorSetLayout::Builder(lbvDevice);
			for (int i = 0; i < inputAttachments.size(); i++) {
				builder.addBinding(i, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
			}
			ppfxSceneDescriptorLayout = builder.build();
		}
		createSceneDescriptors();
		createRenderPass(resolution);
		createPipelineLayout();
		createPipeline(fragmentShader);
	}

	LBVPostProcessingEffect::~LBVPostProcessingEffect() {
		delete ppfxRenderTarget;
		delete ppfxRenderPass;
		for (auto& sub : ppfxSubFramebuffers) {
			for (auto* ppfxFramebuffer : sub) {
				delete ppfxFramebuffer;
			}
		}
	}

	void LBVPostProcessingEffect::render(VkCommandBuffer commandBuffer, const void* pushData, uint32_t layer, uint32_t mipLevel) {
		ppfxRenderPass->beginRenderPass(commandBuffer, ppfxSubFramebuffers[layer][mipLevel]);
		ppfxPipeline->bind(commandBuffer);

		if (inputAttachments.size() != 0) {
			vkCmdBindDescriptorSets(commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				ppfxPipelineLayout->getPipelineLayout(),
				0,
				1,
				&ppfxSceneDescriptorSet,
				0,
				nullptr
			);
		}
		if (pushData) {
			ppfxPush.push(commandBuffer, ppfxPipelineLayout->getPipelineLayout(), pushData);
		}
		vkCmdDraw(commandBuffer, ppfxFramebufferViewType == VK_IMAGE_VIEW_TYPE_CUBE ? 36 : 6, 1, 0, 0);
		ppfxRenderPass->endRenderPass(commandBuffer);
	}

	void LBVPostProcessingEffect::resize(glm::vec2 newResolution, const std::vector<VkDescriptorImageInfo>& newInputAttachments) {
		inputAttachments = newInputAttachments;
		createRenderPass(newResolution);
		createSceneDescriptors();
	}

	void LBVPostProcessingEffect::createPipelineLayout() {
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
		if (inputAttachments.size() != 0) {
			descriptorSetLayouts.push_back(ppfxSceneDescriptorLayout->getDescriptorSetLayout());
		}
		ppfxPush = LBVPushConstant(128, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
		std::vector<VkPushConstantRange> pushConstantRanges{
			ppfxPush.getRange()
		};
		ppfxPipelineLayout = std::make_unique<LBVPipelineLayout>(lbvDevice, pushConstantRanges, descriptorSetLayouts);
	}

	void LBVPostProcessingEffect::createPipeline(const LBVShader& fragmentShader) {
		LBVGraphicsPipelineConfigInfo pipelineConfig{};
		if (!ppfxFramebufferViewType != VK_IMAGE_VIEW_TYPE_CUBE) {
			pipelineConfig.setCullMode(VK_CULL_MODE_BACK_BIT);
		}
		pipelineConfig.disableDepthTest();
		pipelineConfig.pipelineLayout = ppfxPipelineLayout->getPipelineLayout();
		pipelineConfig.renderPass = ppfxRenderPass->getRenderPass();
		ppfxPipeline = std::make_unique<LBVGraphicsPipeline>(
			lbvDevice, std::vector<LBVShader>{
			LBVShader{ LBVShaderType::Vertex, ppfxFramebufferViewType == VK_IMAGE_VIEW_TYPE_CUBE ? "res/shaders/spirv/full_screen_cube.vsh.spv" : "res/shaders/spirv/full_screen.vsh.spv" },
				fragmentShader
			},
			pipelineConfig
		);
	}

	void LBVPostProcessingEffect::createSceneDescriptors() {
		if (inputAttachments.size() != 0) {
			auto writer = LBVDescriptorWriter(*ppfxSceneDescriptorLayout, descriptorPool);
			for (int i = 0; i < inputAttachments.size(); i++) {
				writer.writeImage(i, &inputAttachments[i]);
			}
			writer.build(ppfxSceneDescriptorSet);
		}
	}
	void LBVPostProcessingEffect::createRenderPass(glm::vec2 resolution) {
		if (ppfxRenderTarget) delete ppfxRenderTarget;
		if (ppfxRenderPass) delete ppfxRenderPass;
		for (auto& sub : ppfxSubFramebuffers) {
			for (auto* ppfxFramebuffer : sub) {
				if (ppfxFramebuffer) delete ppfxFramebuffer;
			}
		}
		ppfxSubFramebuffers.clear();

		LBVFramebufferAttachmentCreateInfo createInfo{};
		createInfo.framebufferFormat = ppfxFramebufferFormat;
		createInfo.imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.viewType = ppfxFramebufferViewType;
		createInfo.dimensions = { resolution, 1 };
		createInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		createInfo.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		createInfo.framebufferType = LBVFramebufferAttachmentType::Color;
		createInfo.linearFiltering = true;
		createInfo.layerCount = layerCount;
		createInfo.mipLevels = mipLevels;

		ppfxRenderTarget = new LBVFramebufferAttachment(lbvDevice, createInfo);

		ppfxRenderPass = new LBVRenderPass(lbvDevice, { LBVAttachmentInfo{ppfxRenderTarget, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE }});
		ppfxSubFramebuffers.resize(layerCount);
		for (int i = 0; i < layerCount; i++) {
			ppfxSubFramebuffers[i].resize(mipLevels);
			for (int j = 0; j < mipLevels; j++) {
				ppfxSubFramebuffers[i][j] = new LBVFramebuffer(lbvDevice, ppfxRenderPass, { ppfxRenderTarget }, i, j);
			}
		}
	}
}


