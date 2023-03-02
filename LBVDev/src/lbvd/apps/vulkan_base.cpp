#include "vulkan_base.h"
namespace LittleBigVulkan::Apps {
	VulkanBaseApp::VulkanBaseApp(const char* name) :
		App(name), 
		lbvRenderer(new LBVRenderer(lbvDevice)),
		lbvSwapChain(new LBVSwapChain(lbvDevice, lbvWindow, lbvWindow.getExtent()))
	{
		uint32_t imageCount = lbvSwapChain->getImageCount();
		globalUBODescriptorLayout = LBVDescriptorSetLayout::Builder(lbvDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();
		sceneSSBODescriptorLayout = LBVDescriptorSetLayout::Builder(lbvDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		inFlightPool = LBVDescriptorPool::Builder(lbvDevice)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, imageCount)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, imageCount)
			.setMaxSets(imageCount * 2)
			.build();

		staticPool = LBVDescriptorPool::Builder(lbvDevice)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4096)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4096)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4096)
			.setMaxSets(4096 * 3)
			.build();

		for (int i = 0; i < lbvSwapChain->getImageCount(); i++) {
			InFlightRenderData data{};
			data.uboBuffer = std::make_unique<LBVBuffer>(
				lbvDevice,
				sizeof(GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			data.uboBuffer->map();
			LBVDescriptorWriter(*globalUBODescriptorLayout, *inFlightPool)
				.writeBuffer(0, &data.uboBuffer->getDescriptorInfo())
				.build(data.uboDescriptorSet);

			data.sceneSSBO = std::make_unique<SceneSSBO>(); // allocate this on the heap because it can get f***ing massive
			data.ssboBuffer = std::make_unique<LBVBuffer>(
				lbvDevice,
				sizeof(SceneSSBO),
				1,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			data.ssboBuffer->map();
			LBVDescriptorWriter(*sceneSSBODescriptorLayout, *inFlightPool)
				.writeBuffer(0, &data.ssboBuffer->getDescriptorInfo())
				.build(data.ssboDescriptorSet);
			renderData.push_back(std::move(data));
		}
	}
	VulkanBaseApp::~VulkanBaseApp() {
		delete lbvRenderer;
		delete lbvSwapChain;
		renderData.clear();
	}
	void VulkanBaseApp::run() {
		
	}
}