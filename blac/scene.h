#pragma once

#include <cstdint>
#include <vector>
#include <tuple>

#include "spdlog/spdlog.h"
#include "tinygltf/tiny_gltf.h"

#include "vector3.h"
#include "meshinfo.h"
#include "uv.h"

class Scene {
private:
	int32_t _nobjs;
	std::vector<MeshInfo> _meshInfos;

	// Gets written as int16_t
	std::vector<Vector3<float>> _verts;
	std::vector<Vector3<float>> _normals;
	std::vector<int> _indices;

	std::vector<Vector3<float>> _vertexColors;
	std::vector<UV> _uvs;

	friend class TMD;
public:
	void accept(tinygltf::Model &model);
};