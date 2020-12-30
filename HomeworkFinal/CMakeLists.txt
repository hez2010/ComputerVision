cmake_minimum_required (VERSION 3.15)

project ("classifier")

set(CMAKE_CXX_STANDARD "17")
set(CMAKE_C_STANDARD "11")

include_directories("CImg")
include_directories("libsvm")

aux_source_directory(./ SRC_MAIN)
add_executable (classifier ${SRC_MAIN} ./libsvm/svm.cpp)
set_property(TARGET classifier PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
