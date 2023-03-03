#pragma once
#include <lbv/dll.h>

#include <lbv/vkapi/buffer.h>
#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>
#include <vector>
#include <array>

namespace LittleBigVulkan {
	enum class LBVAxis {
		X,
		Y,
		Z
	};
	
	struct LBVAPI LBVShapeFace {
		std::array<glm::vec2, 3> points;
	};
	struct LBVAPI Vertex {
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
		bool operator==(const Vertex& other) const {
			return position == other.position && normal == other.normal && uv == other.uv;
		}
	};
	struct LBVAPI LBVShapeMesh {
		std::vector<Vertex> vertices{};
	};
	
	class LBVAPI LBVShape {
	public:
		struct LBVAPI Builder {
			Builder() = default;
			std::vector<LBVShapeFace> frontFaces;

			Builder& addTriangle(std::array<glm::vec2, 3> points);
			Builder& setDepth(float depth);
			LBVShapeMesh build();
		private:
			float depth = 0.5f;
		};

		LBVShape(LBVDevice& device, const LBVShapeMesh& mesh);
		~LBVShape();

		LBVShape(const LBVShape&) = delete;
		LBVShape& operator=(const LBVShape&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		template <typename T>
		std::unique_ptr<LBVBuffer> createBuffer(const std::vector<T>& data, bool indexBuffer);

		LBVDevice& lbvDevice;

		uint32_t indexCount;
		std::unique_ptr<LBVBuffer> vertexBuffer;
		std::unique_ptr<LBVBuffer> indexBuffer;
	};
}
