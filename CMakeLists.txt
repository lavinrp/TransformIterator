# CMakeList.txt : CMake project for TransformIterator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project ("TransformIterator")

# Add source to this project's executable.
add_executable (TransformIterator "TransformIteratorTests.cpp" "TransformIterator.h" "catch2/catch.hpp")