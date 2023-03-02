#include "resource_system.h"

namespace LittleBigVulkan {
	ResourceSystem::ResourceSystem(LBVDevice& device, LBVDescriptorPool& descriptorPool) : lbvDevice(device), lbvDescriptorPool(descriptorPool) {
		special_MissingTexture2D = getTexture2D(loadTexture2D("res/textures/missing.png", false, false));
	}
	ResourceSystem::~ResourceSystem() {
		for (auto& [id, model] : modelAssets) {
			delete model;
		}
		for (auto& [id, texture] : texture2DAssets) {
			delete texture;
		}
		for (auto& [id, material] : surfaceMaterialAssets) {
			delete material;
		}
	}
	ResourceID ResourceSystem::loadModel(std::string path) {
		ResourceID id = ResourceID(path);
		auto bdr = LBVModel::Builder();
		bdr.loadModel(path);
		modelAssets[id] = new LBVModel(lbvDevice, bdr);
		return id;
	}
	LBVModel* ResourceSystem::getModel(ResourceID id) {
		auto iter = modelAssets.find(id);
		if (iter == modelAssets.end()) {
			throw std::runtime_error("model '" + id.getAsset() + "'not found");
		} else {
			return (*iter).second;
		}
	}
	TextureResourceIDAttributes ResourceSystem::loadTexture2D(std::string path, bool srgb, bool linearSampler) {
		ResourceID id = ResourceID(path);
		auto bdr = LBVTexture::Builder();
		bdr.loadSTB2DImage(path);
		bdr.srgb = srgb;
		
		TextureResourceIDAttributes attribid{};
		attribid.id = id;
		attribid.srgb = srgb;
		attribid.linearSampler = linearSampler;

		texture2DAssets[attribid] = new LBVTexture2D(lbvDevice, bdr);
		bdr.free();
		return attribid;
	}
	LBVTexture2D* ResourceSystem::getTexture2D(TextureResourceIDAttributes id) {
		auto iter = texture2DAssets.find(id);
		if (iter == texture2DAssets.end()) {
			loadTexture2D(id.id.getAsset(), id.srgb, id.linearSampler);
			return texture2DAssets[id];
		} else {
			return (*iter).second;
		}
	}
	ResourceID ResourceSystem::loadSurfaceMaterial(std::string path) {
		ResourceID id = ResourceID(path);
		LBVSurfaceMaterial* material = new LBVSurfaceMaterial(lbvDevice, lbvDescriptorPool, this);
		material->load(path);
		material->updateParams();
		material->updateTextures();
		surfaceMaterialAssets[id] = material;
		return id;
	}
	LBVSurfaceMaterial* ResourceSystem::getSurfaceMaterial(ResourceID id) {
		auto iter = surfaceMaterialAssets.find(id);
		if (iter == surfaceMaterialAssets.end()) {
			throw std::runtime_error("surface material '" + id.getAsset() + "'not found");
		} else {
			return (*iter).second;
		}
	}
}