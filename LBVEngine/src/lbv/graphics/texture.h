#pragma once
#include <lbv/dll.h>

#include <lbv/vkapi/device.h>

namespace LittleBigVulkan {
    class LBVAPI LBVTexture {
    public:
        struct Builder {
            void loadSTB2DImage(const std::string& path);
            void free();
            int width;
            int height;
            int depth;
            size_t dataSize;
            int layers;
            bool srgb;
            std::vector<void*> pixels;
        };
        LBVTexture(LBVDevice& device);
        virtual ~LBVTexture();

        VkSampler getSampler() const {
            return sampler;
        }
        VkImage getImage() const {
            return image;
        }
        VkImageView getImageView() const {
            return imageView;
        }
        VkDescriptorImageInfo getImageInfo() const {
            return descriptor;
        }
        VkImageLayout getImageLayout() const {
            return layout;
        }
        VkExtent3D getExtent() const {
            return extent;
        }
        VkFormat getFormat() const {
            return format;
        }

        void updateDescriptor();
    protected:
        virtual void createTextureImage(const LBVTexture::Builder& builder) = 0;

        void createTextureImageView(VkImageViewType viewType);
        void createTextureSampler();

        VkDescriptorImageInfo descriptor{};

        LBVDevice& lbvDevice;
        VkImage image{};
        VkDeviceMemory imageMemory{};
        VkImageView imageView{};
        VkSampler sampler{};
        VkFormat format{};
        VkFilter filter{};
        VkSamplerAddressMode addressMode{};
        VkImageLayout layout{};
        uint32_t mipLevels{ 1 };
        uint32_t layerCount{ 1 };
        float anisotropy{};
        VkExtent3D extent{};
    };

}