cmake_minimum_required (VERSION 2.6)
project (olam_hdf5_merger)

find_package(MPI REQUIRED)


include_directories(../../include   ../../    )
include_directories(${MPI_C_INCLUDE_PATH})
include_directories(${MPI_CXX_INCLUDE_PATH})
ADD_LIBRARY(olam_hdf5_merger SHARED olam_hdf5_merger.cpp)
target_link_libraries(olam_hdf5_merger hdf5)
set_property(TARGET olam_hdf5_merger PROPERTY COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_C_COMPILE_FLAGS})

