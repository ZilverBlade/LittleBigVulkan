#pragma once
#include <lbv/rendering/frame_info.h>
#include <lbv/rendering/scene_ssbo.h>
#include <lbv/ecs/level.h>
#include <lbv/ecs/actor.h>
#include <lbv/ecs/components.h>

namespace LittleBigVulkan {
	class LBVAPI LightSystem {
	public:
		void update(FrameInfo& frameInfo, SceneSSBO& sceneBuffer);
	};
}