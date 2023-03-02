#include "descriptors.h"
#include <cassert>

namespace LittleBigVulkan {
	// *************** Descriptor Set Layout Builder *********************

	LBVDescriptorSetLayout::Builder& LBVDescriptorSetLayout::Builder::addBinding(
		uint32_t binding,
		VkDescriptorType descriptorType,
		VkShaderStageFlags stageFlags,
		uint32_t count) {
		assert(bindings.count(binding) == 0 && "Binding already in ulbv");
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		bindings[binding] = layoutBinding;
		return *this;
	}

	std::unique_ptr<LBVDescriptorSetLayout> LBVDescriptorSetLayout::Builder::build() const {
		return std::make_unique<LBVDescriptorSetLayout>(lbvDevice, bindings);
	}

	// *************** Descriptor Set Layout *********************

	LBVDescriptorSetLayout::LBVDescriptorSetLayout(
		LBVDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
		: lbvDevice(device), bindings(bindings) {
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
		for (auto [binding, layoutBinding] : bindings) {
			setLayoutBindings.push_back(layoutBinding);
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();
		
		if (vkCreateDescriptorSetLayout(lbvDevice.getDevice(), &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	LBVDescriptorSetLayout::~LBVDescriptorSetLayout() {
		vkDestroyDescriptorSetLayout(lbvDevice.getDevice(), descriptorSetLayout, nullptr);
	}

	// *************** Descriptor Pool Builder *********************

	LBVDescriptorPool::Builder& LBVDescriptorPool::Builder::addPoolSize(
		VkDescriptorType descriptorType, uint32_t count) {
		poolSizes.push_back({ descriptorType, count });
		return *this;
	}

	LBVDescriptorPool::Builder& LBVDescriptorPool::Builder::setPoolFlags(
		VkDescriptorPoolCreateFlags flags) {
		poolFlags = flags;
		return *this;
	}
	LBVDescriptorPool::Builder& LBVDescriptorPool::Builder::setMaxSets(uint32_t count) {
		maxSets = count;
		return *this;
	}

	std::unique_ptr<LBVDescriptorPool> LBVDescriptorPool::Builder::build() const {
		return std::make_unique<LBVDescriptorPool>(lbvDevice, maxSets, poolFlags, poolSizes);
	}

	// *************** Descriptor Pool *********************

	LBVDescriptorPool::LBVDescriptorPool(
		LBVDevice& device,
		uint32_t maxSets,
		VkDescriptorPoolCreateFlags poolFlags,
		const std::vector<VkDescriptorPoolSize>& poolSizes
	) : lbvDevice(device) {
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		descriptorPoolInfo.flags = poolFlags;
		
		if (vkCreateDescriptorPool(lbvDevice.getDevice(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	LBVDescriptorPool::~LBVDescriptorPool() {
		vkDestroyDescriptorPool(lbvDevice.getDevice(), descriptorPool, nullptr);
	}

	bool LBVDescriptorPool::allocateDescriptor(
		const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.pSetLayouts = &descriptorSetLayout;
		allocInfo.descriptorSetCount = 1;

		if (vkAllocateDescriptorSets(lbvDevice.getDevice(), &allocInfo, &descriptor) != VK_SUCCESS) {
			LBVLOG_ERR("Descriptor pool overfilled! Allocation failed!");
			return false;
		}
		return true;
	}

	void LBVDescriptorPool::freeDescriptors(const std::vector<VkDescriptorSet>& descriptors) const {
		vkFreeDescriptorSets(
			lbvDevice.getDevice(),
			descriptorPool,
			static_cast<uint32_t>(descriptors.size()),
			descriptors.data());
	}

	void LBVDescriptorPool::resetPool() {
		vkResetDescriptorPool(lbvDevice.getDevice(), descriptorPool, 0);
	}

	// *************** Descriptor Writer *********************

	LBVDescriptorWriter::LBVDescriptorWriter(LBVDescriptorSetLayout& setLayout, LBVDescriptorPool& pool)
		: setLayout{ setLayout }, pool{ pool } {}

	LBVDescriptorWriter& LBVDescriptorWriter::writeBuffer(
		uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
		assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = setLayout.bindings[binding];

		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;
		
		writes.push_back(write);
		return *this;
	}

	LBVDescriptorWriter& LBVDescriptorWriter::writeImage(
		uint32_t binding, VkDescriptorImageInfo* imageInfo) {
		assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = setLayout.bindings[binding];

		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo;
		write.descriptorCount = 1;

		writes.push_back(write);
		return *this;
	}

	bool LBVDescriptorWriter::build(VkDescriptorSet& set) {
		bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
		if (!success) {
			return false;
		}
		overwrite(set);
		return true;
	}

	void LBVDescriptorWriter::overwrite(VkDescriptorSet& set) {
		for (auto& write : writes) {
			write.dstSet = set;
		}
		vkUpdateDescriptorSets(pool.lbvDevice.getDevice(), writes.size(), writes.data(), 0, nullptr);
	}

}
