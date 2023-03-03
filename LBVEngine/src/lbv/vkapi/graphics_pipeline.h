#pragma once
#include <lbv/dll.h>
#include <lbv/vkapi/device.h>
#include <lbv/vkapi/shader.h>
namespace LittleBigVulkan {

	struct LBVAPI LBVGraphicsPipelineConfigInfo {
		LBVGraphicsPipelineConfigInfo(uint32_t colorAttachmentCount = 1);
		LBVGraphicsPipelineConfigInfo(const LBVGraphicsPipelineConfigInfo&) = delete;
		LBVGraphicsPipelineConfigInfo& operator=(const LBVGraphicsPipelineConfigInfo&) = delete;

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		VkPipelineViewportStateCreateInfo viewportInfo{};
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
		VkPipelineMultisampleStateCreateInfo multisampleInfo{};
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
		std::vector<VkDynamicState> dynamicStateEnables{};
		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;

		void enableMeshDescriptions();
		void enableShapeDescriptions();

		void enableAlphaBlending(uint32_t attachment = 0, VkBlendOp blendOp = VK_BLEND_OP_ADD);
		void wireframe(float thickness = 1.0f);
		void setCullMode(VkCullModeFlags cullMode);
		void setSampleCount(VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
		void reverseDepth();
		void disableDepthTest();
		void disableDepthWrite();
	};

	class LBVAPI LBVGraphicsPipeline {
	public:
		LBVGraphicsPipeline(
			LBVDevice& device,
			const std::vector<LBVShader>& shaders,
			const LBVGraphicsPipelineConfigInfo& configInfo
		);
		~LBVGraphicsPipeline();
		
		LBVGraphicsPipeline(const LBVGraphicsPipeline&) = delete;
		LBVGraphicsPipeline& operator=(const LBVGraphicsPipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);
	private:
		VkPipeline pipeline;
		LBVDevice& lbvDevice;
	};

}