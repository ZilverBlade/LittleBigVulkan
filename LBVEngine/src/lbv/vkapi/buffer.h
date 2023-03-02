#pragma once
#include <lbv/dll.h>

#include <lbv/vkapi/device.h>

namespace LittleBigVulkan {
    class LBVAPI LBVBuffer {
    public:
        LBVBuffer(
            LBVDevice& device, 
            VkDeviceSize instanceSize, 
            uint32_t instanceCount, 
            VkBufferUsageFlags usageFlags, 
            VkMemoryPropertyFlags memoryPropertyFlags, 
            VkDeviceSize minOffsetAlignment = 1
        );
        ~LBVBuffer();

        static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

        LBVBuffer(const LBVBuffer&) = delete;
        LBVBuffer& operator=(const LBVBuffer&) = delete;

        VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void unmap();

        void writeToBuffer(const void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void writeToIndex(const void* data, int index);

        VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult flushIndex(int index);

        VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult invalidateIndex(int index);

        VkDescriptorBufferInfo getDescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkDescriptorBufferInfo getDescriptorInfoForIndex(int index);

        VkBuffer getBuffer() const { return buffer; }
        void* getMappedMemory() const { return mapped; }
        VkBufferUsageFlags getUsageFlags() const { return usageFlags; }
        VkMemoryPropertyFlags getMemoryPropertyFlags() const { return memoryPropertyFlags; }
        VkDeviceSize getBufferSize() const { return bufferSize; }
        VkDeviceSize getInstanceCount() const { return instanceCount; }
        VkDeviceSize getInstanceSize() const { return instanceSize; }
    private:
        LBVDevice& lbvDevice;
        void* mapped = nullptr;
        VkBuffer buffer = nullptr;
        VkDeviceMemory memory = nullptr;

        VkDeviceSize instanceSize;
        uint32_t instanceCount;
        VkDeviceSize alignmentSize;
        VkDeviceSize bufferSize;

        VkBufferUsageFlags usageFlags;
        VkMemoryPropertyFlags memoryPropertyFlags;
    };
}
