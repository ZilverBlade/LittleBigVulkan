#pragma once
#include <lbv/dll.h>
#include <lbv/vkapi/device.h>
namespace LittleBigVulkan {
	enum class LBVAPI LBVShaderType {
		Vertex,
		Fragment
	};
	class LBVAPI LBVShader {
	public:
		LBVShader(
			LBVShaderType type,
			const char* shaderLocation
		);
		
		~LBVShader();

		VkShaderModule createShaderModule(LBVDevice& device) const;
		VkShaderStageFlagBits getVkShaderStage() const { 
			switch (type) {
			case LittleBigVulkan::LBVShaderType::Vertex:
				return VK_SHADER_STAGE_VERTEX_BIT;
				
			case LittleBigVulkan::LBVShaderType::Fragment:
				return VK_SHADER_STAGE_FRAGMENT_BIT;
			}
		}
	private:
		LBVShaderType type;
		const char* file;
		std::vector<char> buffer;
	};
}