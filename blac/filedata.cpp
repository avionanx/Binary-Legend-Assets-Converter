#include "filedata.h"

FileData::FileData(const char* fileName, int fileFlags) {
	this->_file = std::ofstream(fileName, fileFlags);
}

void FileData::close() {
	this->_file.close();
}

template <typename T>
void FileData::write(T value) {
	this->_file.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template void FileData::write<std::uint8_t>(std::uint8_t value);
template void FileData::write<std::int8_t>(std::int8_t value);
template void FileData::write<std::uint16_t>(std::uint16_t value);
template void FileData::write<std::int16_t>(std::int16_t value);
template void FileData::write<std::uint32_t>(std::uint32_t value);
template void FileData::write<std::int32_t>(std::int32_t value);
