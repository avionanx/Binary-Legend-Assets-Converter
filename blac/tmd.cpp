#include "tmd.h"
#include <limits>
#include <stdexcept>

TMD TMD::fromScene(const Scene& scene)
{
	TMD tmd = {};
	tmd._filePtr = 0xc;
	tmd._subFilePtr1 = 0x0;
	tmd._subFilePtr2 = 0x0;
	tmd._tmdId = 0x41;
	tmd._flags = 0x0;
	tmd._nobjs = scene._meshes.size();

	for (auto& mesh : scene._meshes) {
		for (auto& vertex : mesh._verts) {
			if (
				vertex.x * 4096 > std::numeric_limits<int16_t>::max() ||
				vertex.y * 4096 > std::numeric_limits<int16_t>::max() ||
				vertex.z * 4096 > std::numeric_limits<int16_t>::max() ||
				vertex.x * 4096 < std::numeric_limits<int16_t>::min() ||
				vertex.y * 4096 < std::numeric_limits<int16_t>::min() ||
				vertex.z * 4096 < std::numeric_limits<int16_t>::min()
				) {
				spdlog::error("Vertex at ({},{},{}) is out of range", vertex.x, vertex.y, vertex.z);
				throw std::runtime_error("Vertex out of range.");
			}
			Vector3<int16_t> position = { vertex.x * 4096, vertex.y * 4096, vertex.z * 4096 };
			tmd._verts.push_back(position);
		}
		for (auto& normal : mesh._normals) {
			Vector3<int16_t> normalVector = { normal.x * 4096, normal.y * 4096, normal.z * 4096 };
			tmd._normals.push_back(normalVector);
		}
	}

	int vertexCount = 0;
	int normalCount = 0;
	for (int i = 0; i < scene._meshes.size(); i++) {
		const Mesh mesh = scene._meshes.at(i);

		TMDObjectTable entry = {};
		entry._nVerts = mesh._verts.size();
		entry._nNormals = mesh._normals.size();
		entry._nPrimitives = mesh._indices.size() / 3;

		entry._pVerts = scene._meshes.size() * 0x1c + vertexCount * 0x8;
		entry._pNormals = scene._meshes.size() * 0x1c + mesh._verts.size() * 0x8 + normalCount * 0x4;
		if (mesh.textured) {
			entry._pPrimitives =
				scene._meshes.size() * 0x1c +
				tmd._verts.size() * 0x8 +
				tmd._normals.size() * 0x8 +
				i * 32;
		}
		else {
			entry._pPrimitives =
				scene._meshes.size() * 0x1c +
				tmd._verts.size() * 0x8 +
				tmd._normals.size() * 0x8 +
				i * 24;
		}

		vertexCount += mesh._verts.size();
		normalCount += mesh._normals.size();

		for (int index = 0; index < mesh._indices.size() / 3; index += 3) {
			TMDPrimitive primitiveData = {};

			primitiveData.headers = 0x30040004;// (0b110010) << 24 | mesh.primitiveCount;//(0b10110 << 24) | mesh.vertexCount;
			if (mesh.textured) {
				primitiveData.packetData.push_back(uint8_t(mesh._uvs.at(mesh._indices.at(index)).x * 255));
				primitiveData.packetData.push_back(uint8_t(mesh._uvs.at(mesh._indices.at(index)).y * 255));
				primitiveData.packetData.push_back(uint16_t(0));
				primitiveData.packetData.push_back(uint8_t(mesh._uvs.at(mesh._indices.at(index + 1)).x * 255));
				primitiveData.packetData.push_back(uint8_t(mesh._uvs.at(mesh._indices.at(index + 1)).y * 255));
				primitiveData.packetData.push_back(uint16_t(0));
				primitiveData.packetData.push_back(uint8_t(mesh._uvs.at(mesh._indices.at(index + 2)).x * 255));
				primitiveData.packetData.push_back(uint8_t(mesh._uvs.at(mesh._indices.at(index + 2)).y * 255));
				primitiveData.packetData.push_back(int16_t(0));
			}
			if (mesh._vertexColors.empty()) {
				for (int z = 0; z < 3; z++) {
					primitiveData.packetData.push_back(uint8_t(0x80));
					primitiveData.packetData.push_back(uint8_t(0x80));
					primitiveData.packetData.push_back(uint8_t(0x80));
					primitiveData.packetData.push_back(uint8_t(0x0));
				}
			}
			else {
				for (int vIndex = 0; vIndex < 3; vIndex++) {
					primitiveData.packetData.push_back(uint8_t(mesh._vertexColors.at(mesh._indices.at(index + vIndex)).x * 255));
					primitiveData.packetData.push_back(uint8_t(mesh._vertexColors.at(mesh._indices.at(index + vIndex)).y * 255));
					primitiveData.packetData.push_back(uint8_t(mesh._vertexColors.at(mesh._indices.at(index + vIndex)).z * 255));
					primitiveData.packetData.push_back(uint8_t(0x0));
				}
			}

			if (mesh.textured) {
				primitiveData.packetData.push_back(uint16_t(mesh._indices.at(index)));
				primitiveData.packetData.push_back(uint16_t(mesh._indices.at(index + 1)));
				primitiveData.packetData.push_back(uint16_t(mesh._indices.at(index + 2)));
				primitiveData.packetData.push_back(uint16_t(0)); // padding
			}
			else {
				primitiveData.packetData.push_back(uint16_t(mesh._normals.at(mesh._indices.at(index)).x * 4096));
				primitiveData.packetData.push_back(uint16_t(mesh._indices.at(index)));
				primitiveData.packetData.push_back(uint16_t(mesh._normals.at(mesh._indices.at(index + 1)).y * 4096));
				primitiveData.packetData.push_back(uint16_t(mesh._indices.at(index + 1)));
				primitiveData.packetData.push_back(uint16_t(mesh._normals.at(mesh._indices.at(index + 2)).z * 4096));
				primitiveData.packetData.push_back(uint16_t(mesh._indices.at(index + 2)));
			}
			tmd._primitives.push_back(primitiveData);
		}

		tmd._objectTable.push_back(entry);
	}

	return tmd;
}

void TMD::write(FileData& file) {
	file.write<uint32_t>(this->_filePtr);
	file.write<uint32_t>(this->_subFilePtr1);
	file.write<uint32_t>(this->_subFilePtr2);
	file.write<int32_t>(this->_tmdId);
	file.write<int32_t>(this->_flags);
	file.write<int32_t>(this->_nobjs);

	for (const TMDObjectTable& tmdObject : this->_objectTable) {
		file.write<uint32_t>(tmdObject._pVerts);
		file.write<uint32_t>(tmdObject._nVerts);

		file.write<uint32_t>(tmdObject._pNormals);
		file.write<uint32_t>(tmdObject._nNormals);

		file.write<uint32_t>(tmdObject._pPrimitives);
		file.write<uint32_t>(tmdObject._nPrimitives);

		file.write<uint32_t>(tmdObject._scale);
	}

	for (const Vector3<int16_t>& vertex : this->_verts) {
		file.write<int16_t>(vertex.x);
		file.write<int16_t>(vertex.y);
		file.write<int16_t>(vertex.z);
		file.write<int16_t>(0);
	}

	for (const Vector3<int16_t>& normal : this->_normals) {
		file.write<int16_t>(normal.x);
		file.write<int16_t>(normal.y);
		file.write<int16_t>(normal.z);
		file.write<int16_t>(0);
	}

	for (const TMDPrimitive& primitive : this->_primitives) {
		file.write<int32_t>(primitive.headers);

		for (const auto& data : primitive.packetData) {
			auto& type = data.type();
			if (type == typeid(int8_t)) {
				file.write<int8_t>(std::any_cast<int8_t>(data));
			}
			else if (type == typeid(uint8_t)) {
				file.write<uint8_t>(std::any_cast<uint8_t>(data));
			}
			else if (type == typeid(int16_t)) {
				file.write<int16_t>(std::any_cast<int16_t>(data));
			}
			else if (type == typeid(uint16_t)) {
				file.write<uint16_t>(std::any_cast<uint16_t>(data));
			}
			else if (type == typeid(int32_t)) {
				file.write<int32_t>(std::any_cast<int32_t>(data));
			}
			else if (type == typeid(uint32_t)) {
				file.write<uint32_t>(std::any_cast<uint32_t>(data));
			}
		}
	}

	file.close();
}