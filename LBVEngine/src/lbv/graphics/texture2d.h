#pragma once
#include <lbv/dll.h>

#include <lbv/graphics/texture.h>

namespace LittleBigVulkan {
    class LBVAPI LBVTexture2D : public LBVTexture {
    public:
        LBVTexture2D(LBVDevice& device, const LBVTexture::Builder& builder);
        virtual ~LBVTexture2D();

    protected:
        virtual void createTextureImage(const LBVTexture::Builder& builder) override;
    };

}