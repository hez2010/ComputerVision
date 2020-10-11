cmake_minimum_required (VERSION 3.15)

project ("namecard")

include_directories("CImg")

aux_source_directory(./ SRC_MAIN)
add_executable (namecard ${SRC_MAIN})
set_property(TARGET namecard PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
