cmake_minimum_required (VERSION 3.15)

project ("classifier")

include_directories("CImg")

aux_source_directory(./ SRC_MAIN)
add_executable (classifier ${SRC_MAIN})
set_property(TARGET classifier PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
