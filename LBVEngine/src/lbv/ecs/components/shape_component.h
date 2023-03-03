#pragma once
#include <lbv/dll.h>

#include <lbv/utils/resid.h>
#include <lbv/graphics/shape.h>

namespace LittleBigVulkan {
	inline namespace Components {
		struct ShapeComponent {
			ShapeComponent() = default;
			ShapeComponent(const ShapeComponent&) = default;
			std::shared_ptr<LBVShape> shape{};
		};
	}
}