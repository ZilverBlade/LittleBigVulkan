#pragma once
#include <lbv/dll.h>

#include <lbv/vkapi/buffer.h>
#include <lbv/vkapi/descriptors.h>
#include <lbv/utils/resid.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace LittleBigVulkan {
	class LBVAPI ResourceSystem;
	enum class LBVAPI LBVSurfaceShadingModel {
		Lit = 0x01,
		Unlit = 0x02
	};

	class LBVAPI LBVSurfaceMaterial {
	public:
		LBVSurfaceMaterial(LBVDevice& device, LBVDescriptorPool& descriptorPool, ResourceSystem* resourceSystem);
		~LBVSurfaceMaterial();

		LBVSurfaceMaterial(const LBVSurfaceMaterial&) = delete;
		LBVSurfaceMaterial operator=(const LBVSurfaceMaterial&) = delete;

		void load(const std::string& filepath);
		void bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t descriptorOffset);
		void updateParams();
		void updateTextures();

		LBVSurfaceShadingModel shadingModel = LBVSurfaceShadingModel::Lit;

		ResourceID diffuseMap{};
		ResourceID emissiveMap{};
		ResourceID normalMap{};
		ResourceID specularMap{};
		ResourceID opacityMask{};

		glm::vec3 diffuse{ 1.f };
		glm::vec3 emissive{ 0.f };
		float specular = 1.0f;
		float shininess = 1.0f;
		float normalStrength = 1.0f;

		glm::vec2 uvScale{ 1.f };
		glm::vec2 uvOffset{ 0.f };

		bool translucent = false;
	private:
		void writeDescriptor();

		LBVDevice& lbvDevice;
		LBVDescriptorPool& lbvDescriptorPool;

		std::unique_ptr<LBVBuffer> paramBuffer{};

		VkDescriptorSet descriptorSet{};
		ResourceSystem* resourceSystem;
	};
}
