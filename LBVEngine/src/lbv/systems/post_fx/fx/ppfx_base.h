#pragma once
#include <lbv/dll.h>
#include <lbv/systems/post_fx/post_processing_fx.h>

namespace LittleBigVulkan {
	namespace PostFX {
		class LBVAPI Effect {
		public:
			Effect(
				LBVDevice& device
			) : lbvDevice(device) {}
			virtual ~Effect() {}

			Effect(const Effect&) = delete;
			Effect& operator=(const Effect&) = delete;

			virtual void render(FrameInfo& frameInfo) {}
			virtual void resize(glm::vec2 size, const std::vector<LBVFramebufferAttachment*>& newInputAttachments) {}
			virtual LBVFramebufferAttachment* getAttachment() { return nullptr; }
		protected:
			LBVDevice& lbvDevice;
		};
	}
}