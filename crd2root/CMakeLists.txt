cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(crd2root LANGUAGES CXX)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
   message(STATUS "${PROJECT_NAME} requires an out-of-source build.")
   message(STATUS "Please remove these files from ${CMAKE_BUILD_DIR} first:")
   message(STATUS "  CMakeCache.txt")
   message(STATUS "  CMakesFiles")
   message(STATUS "Once these files are removed, create a separate directory")
   message(STATUS "and tun CMake from there, pointing it to:")
   message(STATUS "  ${CMAKE_SOURCE_DIR}")
   message(FATAL_ERROR "in-source build detected")
endif()

find_package(ROOT)
message(STATUS "ROOT_INCLUDE_DIR=${ROOT_INCLUDE_DIR}")
include_directories(${ROOT_INCLUDE_DIRS})

find_package(SNFrontEndElectronics)
message(STATUS "SNFrontEndElectronics_INCLUDE_DIR=${SNFrontEndElectronics_INCLUDE_DIRS}")
include_directories(${SNFrontEndElectronics_INCLUDE_DIRS})

# - Executable:
add_executable(crd2root	
  src/crd2root.cxx
  )

target_link_libraries(crd2root
  SNFrontEndElectronics::snfee
  ${ROOT_LIBRARIES}
  )

set_target_properties(crd2root
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  ) 
	
install(FILES
  ${CMAKE_BINARY_DIR}/crd2root
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
  PERMISSIONS 
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )
# end
