#pragma once

#include <cstdint>
#include <vector>

#include "vector3.h"
#include "uv.h"

class Mesh {
public:
	std::vector<Vector3<float>> _verts;
	std::vector<Vector3<float>> _normals;

	std::vector<Vector3<float>> _vertexColors;
	std::vector<UV> _uvs;

	std::vector<int> _indices;

	bool textured = false;
};