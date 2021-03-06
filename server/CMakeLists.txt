#add_definitions(-D__INFO)
#add_definitions(-D__DEBUG_ALL)
#add_definitions(-D__BENCHMARKS)

set(DAMARIS_SERVER_LIB_SRC
		${DAMARIS_SOURCE_DIR}/server/Server.cpp
		${DAMARIS_SOURCE_DIR}/server/SecondaryServer.cpp
		${DAMARIS_SOURCE_DIR}/server/ServerC.cpp
		${DAMARIS_SOURCE_DIR}/server/ServerFortran.cpp
		${DAMARIS_SOURCE_DIR}/server/Options.cpp)

set(DAMARIS_SERVER_PRG_SRC
		${DAMARIS_SOURCE_DIR}/xml/Model.cpp
		${DAMARIS_SOURCE_DIR}/server/main.cpp
		${DAMARIS_SOURCE_DIR}/common/Action.cpp
		${DAMARIS_SOURCE_DIR}/common/ActionsManager.cpp
		${DAMARIS_SOURCE_DIR}/common/Parameter.cpp
		${DAMARIS_SOURCE_DIR}/common/ParameterSet.cpp
		${DAMARIS_SOURCE_DIR}/common/SharedMessageQueue.cpp
		${DAMARIS_SOURCE_DIR}/common/SharedMemorySegment.cpp
		${DAMARIS_SOURCE_DIR}/common/Environment.cpp
		${DAMARIS_SOURCE_DIR}/common/Configuration.cpp
		${DAMARIS_SOURCE_DIR}/common/Types.cpp
		${DAMARIS_SOURCE_DIR}/common/Layout.cpp
		${DAMARIS_SOURCE_DIR}/common/MetadataManager.cpp
		${DAMARIS_SOURCE_DIR}/common/Variable.cpp)

add_library (damaris-server ${DAMARIS_SERVER_LIB_SRC})

add_executable (server ${DAMARIS_SOURCE_DIR}/server/main.cpp)

target_link_libraries (server damaris-server damaris xerces-c ${BOOST_LIBRARIES} ${PYTHON_LIBRARIES}  rt dl)

include_directories ("${PROJECT_SOURCE_DIR}/include")


INSTALL(TARGETS
     server
     damaris-server
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

