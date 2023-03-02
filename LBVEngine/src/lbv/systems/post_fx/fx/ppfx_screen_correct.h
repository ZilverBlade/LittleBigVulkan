#pragma once
#include <lbv/dll.h>
#include <lbv/systems/post_fx/fx/ppfx_base.h>

namespace LittleBigVulkan {
	class LBVAPI LBVSwapChain;
	namespace PostFX {
		class LBVAPI ScreenCorrection : public Effect {
		public:
			ScreenCorrection(LBVDevice& device, glm::vec2 size, LBVDescriptorPool& descriptorPool, LBVFramebufferAttachment* inputAttachment, LBVSwapChain* swapChain);
			virtual ~ScreenCorrection();
			virtual void render(FrameInfo& frameInfo);
			virtual void resize(glm::vec2 size, const std::vector<LBVFramebufferAttachment*>& newInputAttachments);
			virtual LBVFramebufferAttachment* getAttachment() { return nullptr; }
		protected:
			void createPipelineLayout();
			void createPipeline(const LBVShader& fragmentShader, VkRenderPass renderPass);
			void createSceneDescriptors();

			LBVDescriptorPool& lbvDescriptorPool;

			LBVFramebufferAttachment* inputAttachment;

			std::unique_ptr<LBVGraphicsPipeline> ppfxPipeline{};
			std::unique_ptr<LBVPipelineLayout> ppfxPipelineLayout{};
			LBVPushConstant ppfxPush{};

			std::unique_ptr<LBVDescriptorSetLayout> ppfxSceneDescriptorLayout{};
			VkDescriptorImageInfo ppfxDescriptorRenderTarget{};
			VkDescriptorSet ppfxSceneDescriptorSet{};
		};
	}
}