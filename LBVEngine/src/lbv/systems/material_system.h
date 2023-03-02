#pragma once
#include <lbv/dll.h>
#include <lbv/vkapi/descriptors.h>

namespace LittleBigVulkan {
	class LBVAPI MaterialSystem {
	public:
		static std::unique_ptr<LBVDescriptorSetLayout>& getMaterialDescriptorSetLayout() {
			return descriptorLayout;
		}
		static void createDescriptorSetLayout(LBVDevice& device) {
			descriptorLayout = LBVDescriptorSetLayout::Builder(device)
				.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT) // params
				.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) // diffuse
				.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) // emissive
				.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) // normal
				.addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) // specular
				.addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT) // (discard) mask
				.build();
		}
		static void destroyDescriptorSetLayout() {
			descriptorLayout = nullptr;
		}
	private:
		static inline std::unique_ptr<LBVDescriptorSetLayout> descriptorLayout;
	};
}