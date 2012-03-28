cmake_minimum_required (VERSION 2.6)
project (olam_hdf5_merger)

find_package(MPI REQUIRED)

find_library(HDF5_LIBRARY NAMES hdf5 PATHS ENV HDF5_PREFIX PATH_SUFFIXES lib)
find_path(HDF5_INCLUDE NAMES hdf5.h PATHS ENV HDF5_PREFIX PATH_SUFFIXES include)
get_filename_component(hdf5_libdir ${HDF5_LIBRARY} PATH)

find_library(DAMARIS_SERVER_LIB 
  NAMES damaris-server 
  PATHS ENV DAMARIS_PREFIX 
    PATH_SUFFIXES server/ lib/)
find_library(DAMARIS_LIB 
  NAMES damaris 
  PATHS ENV DAMARIS_PREFIX 
    PATH_SUFFIXES lib/ client/)

#find_file(DAMARIS_STATIC_LIB 
#  NAMES libdamaris.a 
#  PATHS ENV DAMARIS_PREFIX PATH_SUFFIXES client/ )

get_filename_component(DAMARIS_SERVER_LIBDIR ${DAMARIS_SERVER_LIB} PATH)
get_filename_component(DAMARIS_LIBDIR ${DAMARIS_LIB} PATH)

set(HDF5_LIBDIR ${hdf5_libdir} CACHE PATH "HDF5 LIbrary Path")
set(HDF5_LIBRARIES ${HDF5_LIBRARY} CACHE FILEPATH "HDF5 Lib")
set(HDF5_INCLUDES ${HDF5_INCLUDE} CACHE FILEPATH "HDF5 Include Path")

include_directories(../../include   ../../    )
include_directories(${MPI_C_INCLUDE_PATH})
include_directories(${MPI_CXX_INCLUDE_PATH})
include_directories(${HDF5_INCLUDE})
link_directories(${HDF5_LIBDIR} ${DAMARIS_SERVER_LIBDIR} ${DAMARIS_LIBDIR})

ADD_LIBRARY(olam_hdf5_merger SHARED olam_hdf5_merger.cpp)

target_link_libraries(olam_hdf5_merger hdf5  ${DAMARIS_LIB}  ${DAMARIS_SERVER_LIB}  )
set_property(TARGET olam_hdf5_merger PROPERTY COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_C_COMPILE_FLAGS})



ADD_EXECUTABLE(test_hdf5 test_hdf5.cpp)
target_link_libraries(test_hdf5 hdf5)
