#pragma once
#include <lbv/dll.h>

#include <lbv/utils/resid.h>

namespace LittleBigVulkan {
	inline namespace Components {
		struct MeshComponent {
			MeshComponent() = default;
			MeshComponent(const MeshComponent&) = default;
			ResourceID mesh{};
			std::unordered_map<std::string, ResourceID> materials; // kv = mapTo, ResourceID = surface material 
		};
	}
}