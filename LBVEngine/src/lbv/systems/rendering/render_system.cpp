#include "render_system.h"
namespace LittleBigVulkan {
	RenderSystem::RenderSystem(
		LBVDevice& device, 
		glm::vec2 size
	) : lbvDevice(device) {

	}
	RenderSystem::~RenderSystem() {
	}
	void RenderSystem::init(glm::vec2 size) {
	}
	void RenderSystem::destroy()
	{
	}
	void RenderSystem::renderShapes(FrameInfo& frameInfo, LBVPushConstant push, VkPipelineLayout pipelineLayout, uint32_t descriptorOffset, bool translucent) {
		frameInfo.level->getRegistry().view<Components::TransformComponent, Components::ShapeComponent>().each(
			[&](auto& tfc, auto& spc) {
				ShapeMatrixPush mpush{};
				mpush.transform = tfc.getTransformMatrix();
				push.push(frameInfo.commandBuffer, pipelineLayout, &mpush);

				spc.shape->bind(frameInfo.commandBuffer);
				spc.shape->draw(frameInfo.commandBuffer);		
			}
		);
	}
}