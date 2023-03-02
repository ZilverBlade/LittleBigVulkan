#pragma once
#include <lbv/dll.h>

#include <vulkan/vulkan.h>

namespace LittleBigVulkan {
	enum class LBVAPI LBVQueueType {
		Graphics,
		Compute
	};
	class LBVAPI LBVQueue {
	public:
		VkQueue queue;
	private:
		bool occupied = false;
		LBVQueueType type;
		friend class LBVAPI LBVDevice;
	};
}