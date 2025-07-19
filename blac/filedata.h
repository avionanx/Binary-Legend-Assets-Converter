#pragma once

#include <fstream>
#include <cstdint>

class FileData {
private:
	std::ofstream _file;
public:
	FileData(const char* fileName, int fileFlags);
	void close();

	template<typename T>
	void write(T value);
};