#pragma once

#include <cstdint>
#include <vector>
#include <tuple>

#include "spdlog/spdlog.h"
#include "tinygltf/tiny_gltf.h"

#include "filedata.h"
#include "vector3.h"
#include "tmdobjecttable.h"
#include "tmdprimitive.h"
#include "scene.h"

//oh well
class Scene;

class TMD {
private:
	uint32_t _filePtr;
	uint32_t _subFilePtr1;
	uint32_t _subFilePtr2;
	int32_t _tmdId;
	int32_t _flags;
	int32_t _nobjs;
	std::vector<TMDObjectTable> _objectTable;

	std::vector<Vector3<std::int16_t>> _verts;
	std::vector<Vector3<std::int16_t>> _normals;
	std::vector<TMDPrimitive> _primitives;
public:
	static TMD fromScene(const Scene& model);
	void write(FileData& file);
};
