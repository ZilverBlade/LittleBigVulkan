#pragma once
#include <lbv/systems/rendering/render_system.h>
#include <lbv/ecs/components.h>

namespace LittleBigVulkan {
	class LBVAPI ForwardRenderSystem final : public RenderSystem {
	public:
		ForwardRenderSystem(LBVDevice& device, glm::vec2 resolution, std::vector<VkDescriptorSetLayout> descriptorSetLayouts, VkSampleCountFlagBits msaaSamples);
		virtual ~ForwardRenderSystem();

		virtual void renderEarlyDepth(FrameInfo& frameInfo) override;
		virtual void renderOpaque(FrameInfo& frameInfo) override;
		 
		virtual void beginOpaquePass(FrameInfo& frameInfo) override;
		virtual void endOpaquePass(FrameInfo& frameInfo) override;

		virtual void resize(glm::vec2 size) override;	
	protected:
		virtual void getColorAttachment(LBVFramebufferAttachment** out) override {
			*out = (sampleCount == VK_SAMPLE_COUNT_1_BIT) ? opaqueColorAttachment : opaqueColorResolveAttachment;
		}
		virtual void getDepthAttachment(LBVFramebufferAttachment** out) override {
			*out = depthAttachment;
		}
		virtual void getOpaqueRenderPass(LBVRenderPass** out) override {
			*out = opaqueRenderPass;
		}

		virtual void init(glm::vec2 size) override;
		virtual void destroy() override;

		virtual void createFramebufferAttachments(glm::vec2 size) override;
		virtual void createRenderPasses() override;
		virtual void createFramebuffers() override;
		virtual void createGraphicsPipelines(std::vector<VkDescriptorSetLayout> descriptorSetLayouts) override;

		VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;

		LBVFramebufferAttachment* depthAttachment{};
		LBVFramebufferAttachment* opaqueColorAttachment{};
		LBVFramebufferAttachment* opaqueColorResolveAttachment{};
		LBVFramebuffer* opaqueFramebuffer{};
		LBVRenderPass* opaqueRenderPass{};
		
		LBVPushConstant push{};
		LBVPipelineLayout* opaquePipelineLayout{};
		LBVGraphicsPipeline* opaquePipeline{};
		
		LBVPipelineLayout* earlyDepthPipelineLayout{};
		LBVGraphicsPipeline* earlyDepthPipeline{};
		LBVFramebuffer* earlyDepthFramebuffer{};
		LBVRenderPass* earlyDepthRenderPass{};
	};
}