cmake_minimum_required (VERSION 3.15)

project ("CVHW")

include_directories("CImg")
aux_source_directory(./ SRC_MAIN)

add_executable (CVHW ${SRC_MAIN})
set_property(TARGET CVHW PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
