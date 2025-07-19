#pragma once

#include <cstdint>
#include <vector>

struct TMDObjectTable {
	uint32_t _nVerts;
	uint32_t _pVerts;

	uint32_t _nNormals;
	uint32_t _pNormals;

	uint32_t _nPrimitives;
	uint32_t _pPrimitives;

	uint32_t _scale;
};