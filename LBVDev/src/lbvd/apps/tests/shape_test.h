#pragma once

#include <lbvd/apps/vulkan_base.h>
#include <lbv/ecs/level.h>

namespace LittleBigVulkan::Apps {
	class ShapeTest : public VulkanBaseApp {
	public:
		ShapeTest(const char* name);
		virtual ~ShapeTest();

		virtual void run();
	protected:
		std::shared_ptr<Level> level;
	};
}