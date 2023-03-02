#pragma once
#include <lbv/dll.h>

#include <string>
#include <lbv/utils/uuid.h>

namespace LittleBigVulkan {
	inline namespace Components {
		struct IDComponent {
			IDComponent(const IDComponent&) = default;
			IDComponent(UUID uuid, std::string tag) : uuid(uuid), tag(tag) {}
			UUID getUUID() { 
				return uuid;
			}
			std::string tag;
		private:
			UUID uuid;
			friend class LBVAPI Actor;
			friend class LBVAPI Level;
		};
	}
}