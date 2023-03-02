#include "level.h"
#include <lbv/ecs/actor.h>

namespace LittleBigVulkan {
	Level::Level() : registry(entt::registry())
	{
	}
	Actor Level::createActor(const std::string& name, UUID uuid) {
		Actor actor = Actor(registry.create(), this);
		actor.addComponent<Components::IDComponent>(uuid, name);
		actor.addComponent<Components::TransformComponent>();
		actorUUIDMap[uuid] = actor.handle;
		return actor;
	}
	Actor Level::getActor(UUID uuid) {
		return Actor(actorUUIDMap[uuid], this);
	}
	//Actor Level::getActor(std::string tag)
	//{
	//	return Actor();
	//}
}