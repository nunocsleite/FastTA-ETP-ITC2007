
set(CMAKE_CXX_FLAGS "-fPIC")

# In order to use C++ 11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

cmake_minimum_required(VERSION 2.8)


# For Release build
set(CMAKE_BUILD_TYPE Release)


# For Degug build
## Specify:
##  -DCMAKE_BUILD_TYPE=Debug
## in CMake arguments


# OpenMP for paralelizing code
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")


# About this project
# FastTA applied to Examination Timetabling Problem - ITC2007 benchmark set
project(FastTA-ITC2007)

include_directories(${CMAKE_SOURCE_DIR}/../../ext/include/ParadisEO-2.0/eo/src/)
include_directories(${CMAKE_SOURCE_DIR}/../../ext/include/ParadisEO-2.0/mo/src/)
include_directories(${CMAKE_SOURCE_DIR}/../../ext/include/ParadisEO-2.0/moeo/src/)


include_directories(exes)
include_directories(lib) 
include_directories(lib/chromosome) 

#
# Add subfolders
#
add_subdirectory(lib)
add_subdirectory(exes)
