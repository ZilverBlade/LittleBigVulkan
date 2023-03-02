#pragma once
#include <lbv/dll.h>

#include "buffer.h"
#include <iostream>
#include <cassert>

namespace LittleBigVulkan {
	LBVBuffer::LBVBuffer(
		LBVDevice& device,
		VkDeviceSize instanceSize,
		uint32_t instanceCount,
		VkBufferUsageFlags usageFlags,
		VkMemoryPropertyFlags memoryPropertyFlags,
		VkDeviceSize minOffsetAlignment
	) : lbvDevice(device),
		instanceSize( instanceSize ),
		instanceCount( instanceCount ),
		usageFlags( usageFlags ),
		memoryPropertyFlags( memoryPropertyFlags ) {
		alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
		bufferSize = alignmentSize * instanceCount;

		device.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
	}
	
	LBVBuffer::~LBVBuffer() {
		unmap();
		vkDestroyBuffer(lbvDevice.getDevice(), buffer, nullptr);
		vkFreeMemory(lbvDevice.getDevice(), memory, nullptr);
	}
	
	VkDeviceSize LBVBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
		if (minOffsetAlignment > 0) {
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		}
		return instanceSize;
	}

	VkResult LBVBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
		assert(buffer && memory && "Called map on buffer before create");
		return vkMapMemory(lbvDevice.getDevice(), memory, offset, size, 0, &mapped);
	}
	
	void LBVBuffer::unmap() {
		if (mapped) {
			vkUnmapMemory(lbvDevice.getDevice(), memory);
			mapped = nullptr;
		}
	}
	
	void LBVBuffer::writeToBuffer(const void* data, VkDeviceSize size, VkDeviceSize offset) {
		assert(mapped && "Cannot copy to unmapped buffer");
	
		char* memOffset = (char*)mapped;
		memOffset += offset;
		memcpy(memOffset, data, size == VK_WHOLE_SIZE ? bufferSize - offset : size);
	}

	VkResult LBVBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkFlushMappedMemoryRanges(lbvDevice.getDevice(), 1, &mappedRange);
	}

	VkResult LBVBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkInvalidateMappedMemoryRanges(lbvDevice.getDevice(), 1, &mappedRange);
	}

	VkDescriptorBufferInfo LBVBuffer::getDescriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
		return VkDescriptorBufferInfo{
			buffer,
			offset,
			size,
		};
	}

	void LBVBuffer::writeToIndex(const void* data, int index) {
		writeToBuffer(data, instanceSize, index * alignmentSize);
	}

	VkResult LBVBuffer::flushIndex(int index) {
		return flush(alignmentSize, index * alignmentSize);
	}
	

	VkDescriptorBufferInfo LBVBuffer::getDescriptorInfoForIndex(int index) {
		return getDescriptorInfo(alignmentSize, index * alignmentSize);
	}

	VkResult LBVBuffer::invalidateIndex(int index) {
		return invalidate(alignmentSize, index * alignmentSize);
	}
}
