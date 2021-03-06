set(CLIENT_SIDE_LIBRARIES damaris ${BOOST_LIBRARIES} ${PYTHON_LIBRARIES} xerces-c rt)
set(EMB_SERVER_LIBRARIES damaris-server ${CLIENT_SIDE_LIBRARIES} dl)

#if(CMAKE_Fortran_COMPILER)
#  add_executable (test_fortran ${DAMARIS_SOURCE_DIR}/examples/test.f90)
#  target_link_libraries (test_fortran ${CLIENT_SIDE_LIBRARIES} rt dl)
#  set_property(TARGET test_fortran 
#      PROPERTY LINKER_LANGUAGE Fortran )
#
##  the following two lines are supposed to compile the fortran alloc test,
##  which requires a module. Yet CMake is not able to find the module itself,
##  so the test must be compiled manualy or the module file should be copied 
##  in the examples directory before uncommenting these lines and running
##  "make" again.
#
##  add_executable (test_alloc_fortran ${DAMARIS_SOURCE_DIR}/examples/test_alloc.f90)
##  target_link_libraries (test_alloc_fortran ${CLIENT_SIDE_LIBRARIES} rt dl)
#
#  add_executable (test_chunk_fortran ${DAMARIS_SOURCE_DIR}/examples/test_chunk.f90)
#  set_property(TARGET test_chunk_fortran 
#      PROPERTY LINKER_LANGUAGE Fortran )
#  target_link_libraries (test_chunk_fortran ${CLIENT_SIDE_LIBRARIES} rt dl)
#
#endif(CMAKE_Fortran_COMPILER)

add_executable (test_alloc_c ${DAMARIS_SOURCE_DIR}/examples/test_alloc.c)
target_link_libraries (test_alloc_c ${CLIENT_SIDE_LIBRARIES} rt dl)

add_executable (test_c ${DAMARIS_SOURCE_DIR}/examples/test.c)
target_link_libraries (test_c ${CLIENT_SIDE_LIBRARIES} rt dl)

add_executable (test_event_c ${DAMARIS_SOURCE_DIR}/examples/test_event.c)
target_link_libraries (test_event_c ${CLIENT_SIDE_LIBRARIES} rt dl)

add_executable (test_chunk_c ${DAMARIS_SOURCE_DIR}/examples/test_chunk.c)
target_link_libraries (test_chunk_c ${CLIENT_SIDE_LIBRARIES} rt dl)

if(HAS_MPI)
  add_executable (test_mpi_c ${DAMARIS_SOURCE_DIR}/examples/test_mpi.c)
  target_link_libraries (test_mpi_c ${EMB_SERVER_LIBRARIES} rt dl)

#  if(CMAKE_Fortran_COMPILER)
#    add_executable (test_mpi_fortran ${DAMARIS_SOURCE_DIR}/examples/test_mpi.f90)
#    set_property(TARGET test_mpi_fortran 
#      PROPERTY LINKER_LANGUAGE Fortran )
#    target_link_libraries (test_mpi_fortran ${EMB_SERVER_LIBRARIES} rt dl mpichf90 mpichf90 mpich opa mpl rt pthread)
#  endif(CMAKE_Fortran_COMPILER)
endif(HAS_MPI)
