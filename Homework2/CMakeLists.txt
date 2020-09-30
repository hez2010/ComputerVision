cmake_minimum_required (VERSION 3.15)

project ("canny")

include_directories("CImg")

aux_source_directory(./ SRC_MAIN)
add_executable (canny ${SRC_MAIN})
set_property(TARGET canny PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
