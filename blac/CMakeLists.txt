# CMakeList.txt : CMake project for blac, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (blac "blac.cpp"  "filedata.h" "filedata.cpp" "scene.h" "tmdobjecttable.h" "scene.cpp" "tmdprimitive.h" "tmd.h" "tmd.cpp" "meshinfo.h"  "uv.h")

target_link_libraries("blac" PRIVATE spdlog::spdlog)

target_include_directories("blac" PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tinygltf)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET blac PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
