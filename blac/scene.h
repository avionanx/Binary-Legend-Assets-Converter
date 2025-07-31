#pragma once

#include <cstdint>
#include <vector>

#include "spdlog/spdlog.h"
#include "tinygltf/tiny_gltf.h"

#include "mesh.h"

class Scene {
private:
	int32_t _nobjs;

	// Gets written as int16_t
	std::vector<Mesh> _meshes;

	friend class TMD;
public:
	void accept(tinygltf::Model &model);
};