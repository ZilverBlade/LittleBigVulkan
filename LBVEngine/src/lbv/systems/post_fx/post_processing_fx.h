#pragma once
#include <lbv/dll.h>
#include <lbv/rendering/frame_info.h>
#include <lbv/vkapi/device.h>
#include <lbv/vkapi/framebuffer_attachment.h>
#include <lbv/vkapi/graphics_pipeline.h>
#include <lbv/vkapi/descriptors.h>
#include <lbv/vkapi/pipeline_layout.h>
#include <lbv/vkapi/push_constant.h>

namespace LittleBigVulkan {
	class LBVAPI LBVFramebuffer;
	class LBVAPI LBVRenderPass;
	class LBVAPI LBVPostProcessingEffect {
	public:
		LBVPostProcessingEffect(
			LBVDevice& device,
			glm::vec2 resolution,
			const LBVShader& fragmentShader,
			LBVDescriptorPool& descriptorPool,
			const std::vector<VkDescriptorImageInfo>& inputAttachments,
			VkFormat framebufferFormat,
			VkImageViewType viewType,
			uint32_t layers = 1,
			uint32_t mipLevels = 1
		);
		~LBVPostProcessingEffect();

		LBVPostProcessingEffect(const LBVPostProcessingEffect&) = delete;
		LBVPostProcessingEffect& operator= (const LBVPostProcessingEffect&) = delete;

		void render(VkCommandBuffer commandBuffer, const void* pushData, uint32_t layer = 0, uint32_t mipLevel = 0);
		void resize(glm::vec2 newResolution, const std::vector<VkDescriptorImageInfo>& inputAttachments);

		LBVFramebufferAttachment* getAttachment() {
			return ppfxRenderTarget;
		}
	private:
		void createPipelineLayout();
		void createPipeline(const LBVShader& fragmentShader);
		void createSceneDescriptors();
		void createRenderPass(glm::vec2 resolution);

		LBVDevice& lbvDevice;
		LBVDescriptorPool& descriptorPool;

		uint32_t mipLevels = 1;
		uint32_t layerCount = 1;

		std::vector<std::vector<LBVFramebuffer*>> ppfxSubFramebuffers{};
		LBVRenderPass* ppfxRenderPass{};
		LBVFramebufferAttachment* ppfxRenderTarget{};
		VkFormat ppfxFramebufferFormat;
		VkImageViewType ppfxFramebufferViewType;

		std::vector<VkDescriptorImageInfo> inputAttachments;

		std::unique_ptr<LBVGraphicsPipeline> ppfxPipeline{};
		std::unique_ptr<LBVPipelineLayout> ppfxPipelineLayout{};
		LBVPushConstant ppfxPush{};

		std::unique_ptr<LBVDescriptorSetLayout> ppfxSceneDescriptorLayout{};
		VkDescriptorImageInfo ppfxDescriptorRenderTarget{};
		VkDescriptorSet ppfxSceneDescriptorSet{};
	};
}