#pragma once
#include <cstdint>
#include <any>
#include <vector>

struct TMDPrimitive {
	int32_t headers;
	std::vector<std::any> packetData;
};