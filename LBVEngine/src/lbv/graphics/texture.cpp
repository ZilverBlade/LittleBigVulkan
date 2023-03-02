#pragma once
#include <lbv/dll.h>

#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <lbv/utils/resid.h>
#include <array>

namespace LittleBigVulkan {
    void LBVTexture::Builder::loadSTB2DImage(const std::string& path) {
        int texChannels;
        pixels.resize(1);
        depth = 1;
        pixels[0] = stbi_load(path.c_str(), &width, &height, &texChannels, 4);
        dataSize = width * height * 4;
        layers = 1;
        if (!pixels[0]) {
            throw std::runtime_error("failed to load texture image! Tried to load '" + path + "'");
        }
    }
  
    void LBVTexture::Builder::free() {
        //for (int i = 0; i < layers; i++) {
        //    stbi_image_free(pixels[i]);
        //    pixels[i] = nullptr;
        //}
        //std::free(pixels);
    }

    LBVTexture::LBVTexture(LBVDevice& device) : lbvDevice(device) {
	}
    LBVTexture::~LBVTexture() {
		vkDestroySampler(lbvDevice.getDevice(), sampler, nullptr);
		vkDestroyImageView(lbvDevice.getDevice(), imageView, nullptr);
		vkDestroyImage(lbvDevice.getDevice(), image, nullptr);
		vkFreeMemory(lbvDevice.getDevice(), imageMemory, nullptr);
	}
	void LBVTexture::updateDescriptor() {
		descriptor.sampler = sampler;
		descriptor.imageView = imageView;
		descriptor.imageLayout = layout;
	}
	void LBVTexture::createTextureImageView(VkImageViewType viewType) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = viewType;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = layerCount;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;

		if (vkCreateImageView(lbvDevice.getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}
	}
	void LBVTexture::createTextureSampler() {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = filter;
        samplerInfo.minFilter = filter;

        samplerInfo.addressModeU = addressMode;
        samplerInfo.addressModeV = addressMode;
        samplerInfo.addressModeW = addressMode;

        samplerInfo.anisotropyEnable = true;
        samplerInfo.maxAnisotropy = anisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        // this fields useful for percentage close filtering for shadow maps
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        samplerInfo.mipmapMode = static_cast<VkSamplerMipmapMode>(filter);
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mipLevels);
        
        if (vkCreateSampler(lbvDevice.getDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
	}

}