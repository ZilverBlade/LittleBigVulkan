#include "pipeline_layout.h"

namespace LittleBigVulkan {
	LBVPipelineLayout::LBVPipelineLayout(LBVDevice& device, const std::vector<VkPushConstantRange>& pushConstantRanges, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts) : lbvDevice(device) {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = pushConstantRanges.size();
		pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();
		if (vkCreatePipelineLayout(lbvDevice.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	LBVPipelineLayout::~LBVPipelineLayout() {
		vkDestroyPipelineLayout(lbvDevice.getDevice(), pipelineLayout, nullptr);
	}
}