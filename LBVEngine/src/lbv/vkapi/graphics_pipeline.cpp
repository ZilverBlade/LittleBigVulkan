#pragma once
#include <lbv/dll.h>
#include "graphics_pipeline.h"
#include <cassert>

namespace LittleBigVulkan {

	LBVGraphicsPipelineConfigInfo::LBVGraphicsPipelineConfigInfo(uint32_t colorAttachmentCount) {
		colorBlendAttachments.resize(colorAttachmentCount);
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
		inputAssemblyInfo.flags = 0;

		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = nullptr;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = nullptr;

		//RASTERIZATION
		rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationInfo.depthClampEnable = VK_FALSE;
		rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationInfo.lineWidth = 1.0f;
		rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationInfo.depthBiasEnable = VK_FALSE;
		rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
		rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

		multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleInfo.sampleShadingEnable = VK_FALSE;
		multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleInfo.minSampleShading = 1.0f;           // Optional
		multisampleInfo.pSampleMask = nullptr;             // Optional
		multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

		for (auto& blendAttachment : colorBlendAttachments) {
			blendAttachment.colorWriteMask =
				VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
				VK_COLOR_COMPONENT_A_BIT;
			blendAttachment.blendEnable = VK_FALSE;
			blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
			blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
			blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
			blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
			blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
			blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional
		}

		colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendInfo.logicOpEnable = VK_FALSE;
		colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		colorBlendInfo.attachmentCount = colorAttachmentCount;
		colorBlendInfo.pAttachments = colorBlendAttachments.data();
		colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

		depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilInfo.depthTestEnable = VK_TRUE;
		depthStencilInfo.depthWriteEnable = VK_TRUE;
		depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		depthStencilInfo.stencilTestEnable = VK_FALSE;
		depthStencilInfo.front = {};  // Optional
		depthStencilInfo.back = {};   // Optional

		dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.pDynamicStates = dynamicStateEnables.data();
		dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
		dynamicStateInfo.flags = 0;
		dynamicStateInfo.pNext = nullptr;
	}

	void LBVGraphicsPipelineConfigInfo::enableMeshDescriptions() {
		attributeDescriptions = {
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 }, // position
			{ 1, 1, VK_FORMAT_R32G32_SFLOAT, 0 },    // uv
			{ 2, 2, VK_FORMAT_R32G32B32_SFLOAT, 0 }, // normal
			{ 3, 3, VK_FORMAT_R32G32B32_SFLOAT, 0 }, // tangent
		};
		bindingDescriptions = {
			{ 0, 12, VK_VERTEX_INPUT_RATE_VERTEX }, // position
			{ 1, 8, VK_VERTEX_INPUT_RATE_VERTEX },  // uv
			{ 2, 12, VK_VERTEX_INPUT_RATE_VERTEX }, // normal
			{ 3, 12, VK_VERTEX_INPUT_RATE_VERTEX }, // tangent
		};
	}

	void LBVGraphicsPipelineConfigInfo::enableShapeDescriptions() {
		attributeDescriptions = {
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 }, // position
			{ 1, 0, VK_FORMAT_R32G32_SFLOAT, 12 },    // uv
			{ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, 20 } // normal
		};
		bindingDescriptions = {
			{ 0, 12 + 8 + 12, VK_VERTEX_INPUT_RATE_VERTEX }, // vertex
		};
	}

	void LBVGraphicsPipelineConfigInfo::enableAlphaBlending(uint32_t attachment, VkBlendOp blendOp) {
		colorBlendAttachments[attachment].blendEnable = VK_TRUE;
		colorBlendAttachments[attachment].colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachments[attachment].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachments[attachment].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachments[attachment].colorBlendOp = blendOp;
		colorBlendAttachments[attachment].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachments[attachment].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachments[attachment].alphaBlendOp = blendOp;
	}

	void LBVGraphicsPipelineConfigInfo::wireframe(float thickness) {
		rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;
		rasterizationInfo.lineWidth = thickness;
	}

	void LBVGraphicsPipelineConfigInfo::setCullMode(VkCullModeFlags cullMode) {
		rasterizationInfo.cullMode = cullMode;
	}

	void LBVGraphicsPipelineConfigInfo::setSampleCount(VkSampleCountFlagBits samples) {
		multisampleInfo.rasterizationSamples = samples;
		multisampleInfo.sampleShadingEnable = static_cast<VkBool32>(samples > VK_SAMPLE_COUNT_1_BIT);
	}

	void LBVGraphicsPipelineConfigInfo::reverseDepth() {
		depthStencilInfo.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
	}

	void LBVGraphicsPipelineConfigInfo::disableDepthTest() {
		depthStencilInfo.depthTestEnable = VK_FALSE;
		depthStencilInfo.depthWriteEnable = VK_FALSE;
	}

	void LBVGraphicsPipelineConfigInfo::disableDepthWrite() {
		depthStencilInfo.depthWriteEnable = VK_FALSE;
	}

	LBVGraphicsPipeline::LBVGraphicsPipeline(LBVDevice& device, const std::vector<LBVShader>& shaders, const LBVGraphicsPipelineConfigInfo& configInfo) : lbvDevice(device) {
		assert(configInfo.pipelineLayout != nullptr &&
				"Cannot create graphics pipeline:: no pipelineLayout provided in configInfo");
		assert(configInfo.renderPass != nullptr &&
			"Cannot create graphics pipeline:: no renderPass provided in configInfo");

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
		std::vector<VkShaderModule> shaderModules{};
		shaderStages.reserve(shaders.size());
		shaderModules.reserve(shaders.size());
		for (auto& shader : shaders) {
			VkPipelineShaderStageCreateInfo shaderStage{};
			VkShaderModule module = shader.createShaderModule(lbvDevice);
			shaderModules.push_back(module);
			shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStage.stage = shader.getVkShaderStage();
			shaderStage.module = module;
			shaderStage.pName = "main";
			shaderStage.flags = 0;
			shaderStage.pNext = nullptr;
			shaderStage.pSpecializationInfo = nullptr;
			shaderStages.push_back(shaderStage);
		}

		auto& bindingDescriptions = configInfo.bindingDescriptions;
		auto& attributeDescriptions = configInfo.attributeDescriptions;
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &configInfo.viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;

		pipelineInfo.layout = configInfo.pipelineLayout;
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = nullptr;

		if (vkCreateGraphicsPipelines(lbvDevice.getDevice(), nullptr, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		for (VkShaderModule sm : shaderModules) {
			vkDestroyShaderModule(lbvDevice.getDevice(), sm, nullptr);
		}
	}

	LBVGraphicsPipeline::~LBVGraphicsPipeline() {
		vkDestroyPipeline(lbvDevice.getDevice(), pipeline, nullptr);
	}

	void LBVGraphicsPipeline::bind(VkCommandBuffer commandBuffer) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

}