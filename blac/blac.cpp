#include "spdlog/spdlog.h"
#include <vector>
#include <fstream>

#include "filedata.h"
#include "scene.h"
#include "tmd.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

int main(int argc, char* argv[])
{
	if (argc != 3) {
		spdlog::info("Usage: blac asset.glb output");
		return 1;
	}
	

	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]);

	
	Scene scene = {};
	scene.accept(model);

	FileData file = { argv[2], std::ios::binary | std::ios::trunc };

	TMD tmd = TMD::fromScene(scene);

	tmd.write(file);
	return 0;
}
