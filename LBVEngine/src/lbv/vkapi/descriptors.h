#pragma once
#include <lbv/dll.h>
#include <lbv/vkapi/device.h>
#include <unordered_map>

namespace LittleBigVulkan {
	class LBVAPI LBVDescriptorSetLayout {
	public:
		class LBVAPI Builder {
		public:
			Builder(LBVDevice& device) : lbvDevice{ device } {}

			Builder& addBinding(
				uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1
			);
			std::unique_ptr<LBVDescriptorSetLayout> build() const;
		private:
			LBVDevice& lbvDevice;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

		LBVDescriptorSetLayout(LBVDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~LBVDescriptorSetLayout();

		LBVDescriptorSetLayout(const LBVDescriptorSetLayout&) = delete;
		LBVDescriptorSetLayout& operator=(const LBVDescriptorSetLayout&) = delete;

		VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
	private:
		LBVDevice& lbvDevice;
		VkDescriptorSetLayout descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

		friend class LBVAPI LBVDescriptorWriter;
	};

	class LBVAPI LBVDescriptorPool {
	public:
		class LBVAPI Builder {
		public:
			Builder(LBVDevice& device) : lbvDevice{ device } {}

			Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& setMaxSets(uint32_t count);
			std::unique_ptr<LBVDescriptorPool> build() const;

		private:
			LBVDevice& lbvDevice;
			std::vector<VkDescriptorPoolSize> poolSizes{};
			uint32_t maxSets = 1000;
			VkDescriptorPoolCreateFlags poolFlags = 0;
		};

		LBVDescriptorPool(
			LBVDevice& device,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes
		);
		~LBVDescriptorPool();

		LBVDescriptorPool(const LBVDescriptorPool&) = delete;
		LBVDescriptorPool& operator=(const LBVDescriptorPool&) = delete;

		bool allocateDescriptor(
			const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void freeDescriptors(const std::vector<VkDescriptorSet>& descriptors) const;

		void resetPool();

	private:
		LBVDevice& lbvDevice;
		VkDescriptorPool descriptorPool;

		friend class LBVAPI LBVDescriptorWriter;
	};

	class LBVAPI LBVDescriptorWriter {
	public:
		LBVDescriptorWriter(LBVDescriptorSetLayout& setLayout, LBVDescriptorPool& pool);

		LBVDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		LBVDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);

	private:
		LBVDescriptorSetLayout& setLayout;
		LBVDescriptorPool& pool;
		std::vector<VkWriteDescriptorSet> writes;
	};
} 