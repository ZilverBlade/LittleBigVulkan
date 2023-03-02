#pragma once
#include <lbv/dll.h>
#include <lbv/vkapi/device.h>

namespace LittleBigVulkan {
	class LBVAPI LBVPipelineLayout {
	public:
		LBVPipelineLayout(LBVDevice& device, const std::vector<VkPushConstantRange>& pushConstantRanges = std::vector<VkPushConstantRange>(), const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts = std::vector<VkDescriptorSetLayout>());
		~LBVPipelineLayout();

		LBVPipelineLayout(const LBVPipelineLayout&) = delete;
		LBVPipelineLayout& operator=(const LBVPipelineLayout&) = delete;

		VkPipelineLayout getPipelineLayout() { return pipelineLayout; }
	private:
		VkPipelineLayout pipelineLayout{};
		LBVDevice& lbvDevice;
	};
}