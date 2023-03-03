#pragma once
#include "shape.h"
#include <lbv/utils/hash.h>

namespace std {
	template <>
	struct hash<LittleBigVulkan::Vertex> {
		size_t operator()(LittleBigVulkan::Vertex const& vertex) const {
			size_t seed = 0;
			LittleBigVulkan::hashCombine(seed, vertex.position, vertex.normal, vertex.uv);
			return seed;
		}
	};
}
namespace LittleBigVulkan {
	
	LBVShape::Builder& LBVShape::Builder::addTriangle(std::array<glm::vec2, 3> points) {
		frontFaces.push_back({ points }); // facing towards the camera (-Y = towards the camera)

		return *this;
	}
	LBVShape::Builder& LBVShape::Builder::setDepth(float depth) {
		this->depth = depth;
		return *this;
	}
	LBVShapeMesh LBVShape::Builder::build() {
		LBVShapeMesh mesh{};

		std::vector<glm::vec3> yFaceVertices;
		std::vector<std::array<glm::vec3, 3>> zFaces;

		// draw the face of the shape facing the camera (easy since the triangles input must be correct already)
		yFaceVertices.reserve(frontFaces.size() * 3);
		for (auto& yface : frontFaces) {
			yFaceVertices.push_back({ yface.points[0].x, yface.points[0].y, 0.0 });
			yFaceVertices.push_back({ yface.points[1].x, yface.points[1].y, 0.0 });
			yFaceVertices.push_back({ yface.points[2].x, yface.points[2].y, 0.0 });	
		}

		// discovering the vertices adjacent to the Y face
		for (auto& yface : frontFaces) {
			for (int i = 0; i < 2; i++) { // generate perpendicular faces by chunks of 2 vertices at a time
				// vertices must be ordered clockwise
				zFaces.emplace_back(
					glm::vec3{ yface.points[i].x, yface.points[i].y, 0.0 },
					glm::vec3{ yface.points[i + 1].x, yface.points[i + 1].y, depth },
					glm::vec3{ yface.points[i + 1].x, yface.points[i + 1].y, 0.0 }
				);
				zFaces.emplace_back(
					glm::vec3{ yface.points[i].x, yface.points[i].y, 0.0 },
					glm::vec3{ yface.points[i].x, yface.points[i].y, depth },
					glm::vec3{ yface.points[i + 1].x, yface.points[i + 1].y, depth }
				);
			}
		}

		// just copy over the vertices, these all face toward the camera so the normal vector is (0, 0, 1)
		for (auto& yVtx : yFaceVertices) {
			mesh.vertices.push_back({
				yVtx,
				{ yVtx.x, yVtx.y },
				{ 0.0, 0.0, -1.0 }
			});
		}
		for (auto& zface : zFaces) {
			glm::vec3 normal = glm::normalize(glm::cross(zface[1] - zface[0], zface[2] - zface[0]));
			for (int i = 0; i < 3; i++) {
				mesh.vertices.push_back({
					zface[i],
					{ zface[i].x, zface[i].z }, // looking on the face from "above", the coordinates will be the sort of XY = XZ
					normal
				});
			}
		}

		return mesh;
	}
	LBVShape::LBVShape(LBVDevice& device, const LBVShapeMesh& mesh) : lbvDevice(device) {
		std::map<Vertex, uint16_t> vertexMap;
		std::vector<Vertex> verticesUnique;
		std::vector<uint16_t> indices;

		for (const Vertex& vertex : mesh.vertices) {
			auto iter = vertexMap.find(vertex);
			if (iter != vertexMap.end()) {
				indices.push_back(iter->second);
			} else {
				assert(vertexMap.size() <= UINT16_MAX && "Shape index exceeded limit!");
				vertexMap[vertex] = vertexMap.size();
				verticesUnique.push_back(vertex);
			}
		}	
		indexCount = indices.size();
		vertexBuffer = createBuffer(verticesUnique, false);
		indexBuffer = createBuffer(indices, true);
	}
	LBVShape::~LBVShape() {

	}

	void LBVShape::bind(VkCommandBuffer commandBuffer) {
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
		VkDeviceSize offsets[]{ 0 };
		VkBuffer buffers[]{ vertexBuffer->getBuffer() };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	void LBVShape::draw(VkCommandBuffer commandBuffer) {
		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
	}

	template<typename T>
	inline std::unique_ptr<LBVBuffer> LBVShape::createBuffer(const std::vector<T>& data, bool indexBuffer) {
		std::unique_ptr<LBVBuffer> stagingBuffer = std::make_unique<LBVBuffer>(
			lbvDevice,
			sizeof(T),
			data.size(),
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT | indexBuffer ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT : VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		stagingBuffer->map();
		stagingBuffer->writeToBuffer(data.data());

		std::unique_ptr<LBVBuffer> dstBuffer = std::make_unique<LBVBuffer>(
			lbvDevice,
			sizeof(T),
			data.size(),
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | indexBuffer ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT : VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		VkCommandBuffer commandBuffer = lbvDevice.beginSingleTimeCommands();
		lbvDevice.copyBuffer(commandBuffer, stagingBuffer->getBuffer(), dstBuffer->getBuffer(), stagingBuffer->getBufferSize());
		lbvDevice.endSingleTimeCommands(commandBuffer);
		return dstBuffer;
	}
}
