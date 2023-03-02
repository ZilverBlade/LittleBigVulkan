#pragma once
#include <lbv/dll.h>

#include "surface_material.h"
#include <lbv/systems/resource_system.h>
#include <lbv/systems/material_system.h>
#include <numeric>
#include <simdjson.h>

namespace LittleBigVulkan {
	// copied from "res/shaders/surface_material.glsl" 
	const uint32_t SURFACE_MATERIAL_TEXTURE_DIFFUSE_BIT = 0x01;
	const uint32_t SURFACE_MATERIAL_TEXTURE_EMISSIVE_BIT = 0x02;
	const uint32_t SURFACE_MATERIAL_TEXTURE_NORMAL_BIT = 0x04;
	const uint32_t SURFACE_MATERIAL_TEXTURE_SPECULAR_BIT = 0x08;
	const uint32_t SURFACE_MATERIAL_TEXTURE_MASK_BIT = 0x80;
	const uint32_t SURFACE_MATERIAL_TEXTURE_OPACITY_BIT = 0x100;
	struct SurfaceMaterialBufferInfo {
		glm::vec4 diffuse;
		glm::vec4 emission;
		float specular;
		float shininess;
		glm::vec2 uvScale;
		glm::vec2 uvOffset;
		uint32_t textureFlags;
		uint32_t shadingModelFlag;
	};

	LBVSurfaceMaterial::LBVSurfaceMaterial(LBVDevice& device, LBVDescriptorPool& descriptorPool, ResourceSystem* resourceSystem) :
		lbvDevice(device), lbvDescriptorPool(descriptorPool), resourceSystem(resourceSystem) {
		paramBuffer = std::make_unique<LBVBuffer>(
			lbvDevice,
			sizeof(SurfaceMaterialBufferInfo),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);
		paramBuffer->map();

		LBVDescriptorWriter::LBVDescriptorWriter(*MaterialSystem::getMaterialDescriptorSetLayout(), lbvDescriptorPool)
			.writeBuffer(0, &paramBuffer->getDescriptorInfo())
			.writeImage(1, &resourceSystem->getMissingTexture2D()->getImageInfo())
			.writeImage(2, &resourceSystem->getMissingTexture2D()->getImageInfo())
			.writeImage(3, &resourceSystem->getMissingTexture2D()->getImageInfo())
			.writeImage(4, &resourceSystem->getMissingTexture2D()->getImageInfo())
			.writeImage(5, &resourceSystem->getMissingTexture2D()->getImageInfo())
			.writeImage(6, &resourceSystem->getMissingTexture2D()->getImageInfo())
			.writeImage(7, &resourceSystem->getMissingTexture2D()->getImageInfo())
			.writeImage(8, &resourceSystem->getMissingTexture2D()->getImageInfo())
			.build(descriptorSet);
	}
	LBVSurfaceMaterial::~LBVSurfaceMaterial() {

	}
	
	void LBVSurfaceMaterial::load(const std::string& filepath) {
		simdjson::dom::parser parser;
		simdjson::padded_string json = simdjson::padded_string::load(filepath);
		const auto& material = parser.parse(json);
		if (material.error() != simdjson::error_code::SUCCESS) {
			throw std::runtime_error("simdjson parser error");
		}
		if (material["class LBVAPI"].get_string().value() != "surface") 
			throw std::runtime_error("wrong material class LBVAPI typed loaded as surface material!");
		if (material["model"].get_string().value() == "lit")
			this->shadingModel = LBVSurfaceShadingModel::Lit;
		if (material["model"].get_string().value() == "unlit")
			this->shadingModel = LBVSurfaceShadingModel::Unlit;

		this->doubleSided = material["doubleSided"].get_bool().value();
		this->translucent = material["translucent"].get_bool().value();
		
		const auto& config = material["config"];
		if (config["diffuse"].error() == simdjson::error_code::SUCCESS)
			this->diffuse = {
				config["diffuse"].get_array().at(0).get_double().value(),
				config["diffuse"].get_array().at(1).get_double().value(),
				config["diffuse"].get_array().at(2).get_double().value()
			};
		if (config["emissive"].error() == simdjson::error_code::SUCCESS)
			this->emissive = {
				config["emissive"].get_array().at(0).get_double().value(),
				config["emissive"].get_array().at(1).get_double().value(),
				config["emissive"].get_array().at(2).get_double().value()
			};
		if (config["specular"].error() == simdjson::error_code::SUCCESS)
			this->specular = config["specular"].get_double().value();
		if (config["shininess"].error() == simdjson::error_code::SUCCESS)
			this->shininess = config["shininess"].get_double().value();
	
		if (config["diffuseTexture"].error() == simdjson::error_code::SUCCESS)
			this->diffuseTexture = std::string(config["diffuseTexture"].get_string().value());
		if (config["emissiveTexture"].error() == simdjson::error_code::SUCCESS)
			this->emissiveTexture = std::string(config["emissiveTexture"].get_string().value());
		if (config["normalTexture"].error() == simdjson::error_code::SUCCESS)
			this->normalTexture = std::string(config["normalTexture"].get_string().value());
		if (config["specularTexture"].error() == simdjson::error_code::SUCCESS)
			this->specularTexture = std::string(config["specularTexture"].get_string().value());
		if (config["opacityMask"].error() == simdjson::error_code::SUCCESS)
			this->maskTexture = std::string(config["opacityMask"].get_string().value());

		if (material["uvScale"].error() == simdjson::error_code::SUCCESS)
			this->uvScale = { 
				material["uvScale"].get_array().at(0).get_double().value(),
				material["uvScale"].get_array().at(1).get_double().value()
			};
		if (material["uvOffset"].error() == simdjson::error_code::SUCCESS)
			this->uvOffset = {
				material["uvOffset"].get_array().at(0).get_double().value(),
				material["uvOffset"].get_array().at(1).get_double().value()
		};
	}
	void LBVSurfaceMaterial::bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t descriptorOffset) {
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			descriptorOffset,
			1,
			&descriptorSet,
			0,
			nullptr
		);
	}
	void LBVSurfaceMaterial::updateParams() {
		SurfaceMaterialBufferInfo bufferInfo{};
		bufferInfo.diffuse = { this->diffuse, 0 };
		bufferInfo.emission = { this->emissive, 0 };
		bufferInfo.specular = this->specular;
		bufferInfo.shininess = this->shininess;

		bufferInfo.uvOffset = this->uvOffset;
		bufferInfo.uvScale = this->uvScale;
		bufferInfo.shadingModelFlag = static_cast<uint32_t>(this->shadingModel);

		uint32_t textureFlags = 0;
		if (diffuseTexture) textureFlags |= SURFACE_MATERIAL_TEXTURE_DIFFUSE_BIT;
		if (emissiveTexture) textureFlags |= SURFACE_MATERIAL_TEXTURE_EMISSIVE_BIT;
		if (normalTexture) textureFlags |= SURFACE_MATERIAL_TEXTURE_NORMAL_BIT;
		if (specularTexture) textureFlags |= SURFACE_MATERIAL_TEXTURE_SPECULAR_BIT;
		if (maskTexture) textureFlags |= SURFACE_MATERIAL_TEXTURE_MASK_BIT;
		bufferInfo.textureFlags = textureFlags;

		paramBuffer->writeToBuffer(&bufferInfo, sizeof(SurfaceMaterialBufferInfo));
		paramBuffer->flush();
	}
	void LBVSurfaceMaterial::updateTextures() {
		updateParams();

		writeDescriptor();
	}
	void LBVSurfaceMaterial::writeDescriptor() {
		auto writer = LBVDescriptorWriter::LBVDescriptorWriter(*MaterialSystem::getMaterialDescriptorSetLayout(), lbvDescriptorPool)
			.writeBuffer(0, &paramBuffer->getDescriptorInfo())
			.writeImage(1, &resourceSystem->getMissingTexture2D()->getImageInfo())
			.writeImage(2, &resourceSystem->getMissingTexture2D()->getImageInfo())
			.writeImage(3, &resourceSystem->getMissingTexture2D()->getImageInfo())
			.writeImage(4, &resourceSystem->getMissingTexture2D()->getImageInfo())
			.writeImage(5, &resourceSystem->getMissingTexture2D()->getImageInfo());

		if (diffuseTexture) writer.writeImage(1, &resourceSystem->getTexture2D({diffuseTexture, true, true})->getImageInfo());
		if (emissiveTexture) writer.writeImage(2, &resourceSystem->getTexture2D({ emissiveTexture, true, true })->getImageInfo());
		if (normalTexture) writer.writeImage(3, &resourceSystem->getTexture2D({ normalTexture, false, true })->getImageInfo());
		if (specularTexture) writer.writeImage(4, &resourceSystem->getTexture2D({ specularTexture, false, true })->getImageInfo());
		if (maskTexture) writer.writeImage(5, &resourceSystem->getTexture2D({ maskTexture, false, true })->getImageInfo());

		writer.build(descriptorSet);
	}
}
