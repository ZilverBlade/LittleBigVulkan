#pragma once
#include <lbv/dll.h>
#include <lbv/utils/uuid.h>

namespace LittleBigVulkan {
	inline namespace Components {
		struct SkyboxComponent {
			UUID skyLightActorID;
		};
	}
}