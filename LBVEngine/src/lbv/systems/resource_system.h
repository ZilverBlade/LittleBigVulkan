#pragma once
#include <lbv/dll.h>

#include <unordered_map>
#include <lbv/utils/resid.h>
#include <lbv/graphics/model.h>
#include <lbv/graphics/texture2d.h>
#include <lbv/graphics/surface_material.h>

namespace LittleBigVulkan {
	struct TextureResourceIDAttributes {
		ResourceID id{};
		bool srgb{};
		bool linearSampler{};
		bool operator==(LittleBigVulkan::TextureResourceIDAttributes const& other) const {
			return id.getID() == other.id.getID() && srgb == other.srgb && linearSampler == other.linearSampler;
		}
	};
}
namespace std {
	template<>
	struct hash<LittleBigVulkan::TextureResourceIDAttributes> {
		size_t operator()(LittleBigVulkan::TextureResourceIDAttributes const& attribid) const {
			return attribid.id.getID() << 2 || attribid.srgb << 1 || attribid.linearSampler;
		}
	};
}

namespace LittleBigVulkan {
	class LBVAPI ResourceSystem {
	public:
		ResourceSystem(LBVDevice& device, LBVDescriptorPool& descriptorPool);
		~ResourceSystem();

		ResourceSystem(const ResourceSystem&) = delete;
		ResourceSystem& operator=(const ResourceSystem&) = delete;

		ResourceID loadModel(std::string path);
		LBVModel* getModel(ResourceID id);

		TextureResourceIDAttributes loadTexture2D(std::string path, bool srgb, bool linearSampler);
		LBVTexture2D* getTexture2D(TextureResourceIDAttributes id);

		ResourceID loadSurfaceMaterial(std::string path);
		LBVSurfaceMaterial* getSurfaceMaterial(ResourceID id);

		LBVTexture2D* getMissingTexture2D() {
			return special_MissingTexture2D;
		}
	private:
		LBVDevice& lbvDevice;
		LBVDescriptorPool& lbvDescriptorPool;

		std::unordered_map<ResourceID, LBVModel*> modelAssets;
		std::unordered_map<TextureResourceIDAttributes, LBVTexture2D*> texture2DAssets;
		std::unordered_map<ResourceID, LBVSurfaceMaterial*> surfaceMaterialAssets;

		LBVTexture2D* special_MissingTexture2D;
	};
}