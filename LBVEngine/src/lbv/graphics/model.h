#pragma once
#include <lbv/dll.h>

#include <lbv/vkapi/buffer.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <unordered_map>
extern "C" {
	struct aiNode;
	struct aiScene;
	struct aiMesh;
}
namespace LittleBigVulkan {
	class LBVAPI LBVModel {
	public:
		struct Submesh {
			std::unique_ptr<LBVBuffer> vertexPositionBuffer;
			std::unique_ptr<LBVBuffer> vertexUVBuffer;
			std::unique_ptr<LBVBuffer> vertexNormalBuffer;
			std::unique_ptr<LBVBuffer> vertexTangentsBuffer;
			std::unique_ptr<LBVBuffer> indexBuffer;

			uint32_t vertexCount{};
			uint32_t indexCount{};
		};
		struct Builder {
			struct Submesh {
				std::vector<glm::vec3> vertexPositions{};
				std::vector<glm::vec2> vertexUVs{};
				std::vector<glm::vec3> vertexNormals{};
				std::vector<glm::vec3> vertexTangents{};
				std::vector<uint32_t> indices{};
			};
			std::unordered_map<std::string, Submesh> submeshes{};
			bool loadModel(const std::string &filepath);
			void processNode(aiNode* node, const aiScene* scene);
			void loadSubmesh(aiMesh* mesh, const aiScene* scene);
			std::string format;
		};

		LBVModel(LBVDevice& device, const LBVModel::Builder& builder);
		~LBVModel();

		LBVModel(const LBVModel &) = delete;
		LBVModel &operator=(const LBVModel &) = delete;

		void bind(VkCommandBuffer commandBuffer, const std::string& submeshName);
		void draw(VkCommandBuffer commandBuffer);
		
		std::vector<std::string> getSubmeshes();
	private:
		template <typename T>
		std::unique_ptr<LBVBuffer> createBuffer(const std::vector<T> & data);

		LBVDevice &lbvDevice;

		std::string boundSubmesh{};
		std::unordered_map<std::string, Submesh> submeshes{};
	};
}
