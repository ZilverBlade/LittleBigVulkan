#pragma once
#include <lbv/vk.h>
#include <glm/glm.hpp>
#include <lbv/rendering/frame_info.h>
#include <lbv/ecs/components.h>

namespace LittleBigVulkan {
	struct ShapeMatrixPush {
		glm::mat4 transform;
	};

	class LBVAPI RenderSystem  {
	public:
		RenderSystem(LBVDevice& device, glm::vec2 resolution);
		virtual ~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;
		RenderSystem(RenderSystem&&) = delete;
		RenderSystem& operator=(RenderSystem&&) = delete;

		virtual void renderEarlyDepth(FrameInfo& frameInfo) {}
		virtual void renderOpaque(FrameInfo& frameInfo) {}
		virtual void renderTranslucent(FrameInfo& frameInfo) {}
		virtual void renderSkybox(FrameInfo& frameInfo) {}
		 
		virtual void compositeData(FrameInfo& frameInfo) {}
		 
		virtual void beginOpaquePass(FrameInfo& frameInfo) {}
		virtual void endOpaquePass(FrameInfo& frameInfo) {}
		virtual void beginTranslucentPass(FrameInfo& frameInfo) {}
		virtual void endTranslucentPass(FrameInfo& frameInfo) {}
		virtual void beginCompositionPass(FrameInfo& frameInfo) {}
		virtual void endCompositionPass(FrameInfo& frameInfo){}

		LBVFramebufferAttachment* getColorAttachment() {
			LBVFramebufferAttachment* attachment{};
			getColorAttachment(&attachment);
			return attachment;
		}
		LBVFramebufferAttachment* getDepthAttachment() {
			LBVFramebufferAttachment* attachment{};
			getDepthAttachment(&attachment);
			return attachment;
		}
		virtual void resize(glm::vec2 size) {}

		LBVRenderPass* getOpaqueRenderPass() {
			LBVRenderPass* pass{};
			getOpaqueRenderPass(&pass);
			return pass;
		}
		LBVRenderPass* getTransparencyRenderPass() { return nullptr; }
		LBVRenderPass* getCompositionRenderPass() { return nullptr; }

	protected:
		void renderShapes(FrameInfo& frameInfo, LBVPushConstant push, VkPipelineLayout pipelineLayout, uint32_t descriptorOffset, bool translucent);

		virtual void getColorAttachment(LBVFramebufferAttachment** out) {}
		virtual void getDepthAttachment(LBVFramebufferAttachment** out) {}
		virtual void getOpaqueRenderPass(LBVRenderPass** out) {}

		virtual void init(glm::vec2 size);
		virtual void destroy();

		virtual void createFramebufferAttachments(glm::vec2 size) {}
		virtual void createRenderPasses() {}
		virtual void createFramebuffers() {}
		virtual void createGraphicsPipelines(std::vector<VkDescriptorSetLayout> descriptorSetLayouts) {}

		LBVDevice& lbvDevice;
	};
}