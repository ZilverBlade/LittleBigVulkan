#pragma once
#include <lbv/dll.h>

#include <entt/entt.hpp>
#include <lbv/utils/uuid.h>

namespace LittleBigVulkan {
	class LBVAPI Actor;
	class LBVAPI Level {
	public:
		Level();

		Level(const Level&) = delete;
		Level& operator=(const Level&) = delete;

		Actor createActor(const std::string& name, UUID uuid = UUID());
		Actor getActor(UUID uuid);
		//Actor getActor(std::string tag);

		entt::registry& getRegistry() { return registry; }
	private:
		std::unordered_map<UUID, entt::entity> actorUUIDMap{};

		entt::registry registry;
		friend class LBVAPI Actor;
	};
}