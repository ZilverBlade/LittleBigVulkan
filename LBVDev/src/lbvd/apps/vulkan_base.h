#pragma once

#include <lbvd/apps/app.h>

#include <lbv/vkapi/swap_chain.h>
#include <lbv/vkapi/renderer.h>
#include <lbv/vkapi/graphics_pipeline.h>
#include <lbv/vkapi/pipeline_layout.h>
#include <lbv/vkapi/descriptors.h>
#include <lbv/vkapi/buffer.h>
#include <lbv/rendering/global_ubo.h>
#include <lbv/rendering/scene_ssbo.h>

namespace LittleBigVulkan::Apps {
	struct InFlightRenderData {
		std::unique_ptr<LBVBuffer> uboBuffer{};
		VkDescriptorSet uboDescriptorSet{};
		std::unique_ptr<LBVBuffer> ssboBuffer{};
		VkDescriptorSet ssboDescriptorSet{};

		std::unique_ptr<SceneSSBO> sceneSSBO{};
	};
	class VulkanBaseApp : public App {
	public:
		VulkanBaseApp(const char* name);
		virtual ~VulkanBaseApp();

		virtual void run();
	protected:
		LBVRenderer* lbvRenderer;
		LBVSwapChain* lbvSwapChain;
		std::unique_ptr<LBVDescriptorPool> inFlightPool{};
		std::unique_ptr<LBVDescriptorPool> staticPool{};
		std::vector<InFlightRenderData> renderData{};
		std::unique_ptr<LBVDescriptorSetLayout> globalUBODescriptorLayout{};
		std::unique_ptr<LBVDescriptorSetLayout> sceneSSBODescriptorLayout{};
	};
}