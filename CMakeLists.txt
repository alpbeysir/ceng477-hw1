# CMakeList.txt : CMake project for ceng477-hw1-linux, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("main")

# Add source to this project's executable.
file (GLOB a_src "*.h" "*.cpp")
add_executable (ceng477-hw1-linux ${a_src})

add_compile_options(" -Wno-ignored-attributes")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ceng477-hw1-linux PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
