#include "scene.h"

#include <iostream>
#include <stdexcept>
#include <cmath>

void Scene::accept(tinygltf::Model &model) {

	this->_nobjs = model.meshes.size();

	spdlog::info("Beginning parse");
	for (const auto& mesh : model.meshes) {
		spdlog::info("Parsing mesh {}", mesh.name);

		MeshInfo meshInfo = {};

		uint32_t primitiveCount = 0;
		uint32_t vertexCount = 0;
		uint32_t normalCount = 0;


		for (const auto& primitive : mesh.primitives) {
			if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
				const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.at("POSITION")];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = model.buffers[view.buffer];
				const float* positions = reinterpret_cast<const float*>(&buffer.data[view.byteOffset + accessor.byteOffset]);

				vertexCount += accessor.count;
				for (size_t i = 0; i < accessor.count; ++i) {
					this->_verts.push_back(Vector3{ (positions[i * 3 + 0]), (positions[i * 3 + 1]), (positions[i * 3 + 2]) });
				}
			}
			if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
				const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = model.buffers[view.buffer];

				const float* uvs = reinterpret_cast<const float*>(&buffer.data[view.byteOffset + accessor.byteOffset]);

				for (size_t i = 0; i < accessor.count; ++i) {
					float u = uvs[i * 2 + 0];
					float v = uvs[i * 2 + 1];
					this->_uvs.push_back({ u, v });
				}
			}
			if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
				const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.at("COLOR_0")];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = model.buffers[view.buffer];

				size_t numComponents = accessor.type == TINYGLTF_TYPE_VEC4 ? 4 : 3;

				const float* colors = reinterpret_cast<const float*>(&buffer.data[view.byteOffset + accessor.byteOffset]);

				for (size_t i = 0; i < accessor.count; ++i) {
					float r = colors[i * numComponents + 0];
					float g = colors[i * numComponents + 1];
					float b = colors[i * numComponents + 2];
					//float a = (numComponents == 4) ? colors[i * numComponents + 3] : 1.0f;
					this->_vertexColors.push_back({ r, g, b });
				}
			}
			/*
			* Modern formats have normals for each vertex
			* TMDs have normals for primitives
			* This calculates surface normals from vertex positions
			*/
			
			if (primitive.indices >= 0) {
				const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
				const tinygltf::BufferView& indexView = model.bufferViews[indexAccessor.bufferView];
				const tinygltf::Buffer& indexBuffer = model.buffers[indexView.buffer];
				const unsigned char* indexData = &indexBuffer.data[indexView.byteOffset + indexAccessor.byteOffset];

				primitiveCount += indexAccessor.count/ 3;
				normalCount += indexAccessor.count / 3;

				for (size_t i = 0; i < indexAccessor.count; i += 3) {
					uint32_t index0 = 0;
					uint32_t index1 = 0;
					uint32_t index2 = 0;
					switch (indexAccessor.componentType) {
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
						index0 = static_cast<uint32_t>(indexData[i]);
						index1 = static_cast<uint32_t>(indexData[i + 1]);
						index2 = static_cast<uint32_t>(indexData[i + 2]);
						break;
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
						index0 = static_cast<const uint16_t*>(static_cast<const void*>(indexData))[i];
						index1 = static_cast<const uint16_t*>(static_cast<const void*>(indexData))[i + 1];
						index2 = static_cast<const uint16_t*>(static_cast<const void*>(indexData))[i + 2];
						break;
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
						index0 = static_cast<const uint32_t*>(static_cast<const void*>(indexData))[i];
						index1 = static_cast<const uint32_t*>(static_cast<const void*>(indexData))[i + 1];
						index2 = static_cast<const uint32_t*>(static_cast<const void*>(indexData))[i + 2];
						break;
					}
					this->_indices.push_back(index0);
					this->_indices.push_back(index1);
					this->_indices.push_back(index2);

					const Vector3<float> edge0 = this->_verts.at(index1).sub(this->_verts.at(index0));
					const Vector3<float> edge1 = this->_verts.at(index2).sub(this->_verts.at(index0));

					const Vector3<float> primitiveNormal = edge0.cross(edge1).normalize();

					this->_normals.push_back(primitiveNormal);
				}
			}

			meshInfo.vertexCount = vertexCount;
			meshInfo.normalCount = normalCount;
			meshInfo.primitiveCount = primitiveCount;
			this->_meshInfos.push_back(meshInfo);
		}
	}
	spdlog::info("Parsing complete");
}





