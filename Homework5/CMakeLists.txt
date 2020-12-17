cmake_minimum_required (VERSION 3.15)
project ("hw5")

set(CMAKE_CXX_STANDARD "17")
set(CMAKE_C_STANDARD "11")

include_directories("CImg")
include_directories("libsvm")

add_executable (hw5 main.cpp DataReader.cpp ./libsvm/svm.cpp )

set_property(TARGET hw5 PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
