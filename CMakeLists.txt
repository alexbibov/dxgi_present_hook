# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.19)

project ("D3D11PresentHook")

# Include sub-projects.
add_subdirectory ("present_hook")
add_subdirectory ("injector")
