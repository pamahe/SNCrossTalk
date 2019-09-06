cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(WaveformDrawer LANGUAGES CXX)

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

find_package(SNFrontEndElectronics)
message(STATUS "SNFrontEndElectronics_INCLUDE_DIR=${SNFrontEndElectronics_INCLUDE_DIR}")
include_directories(${SNFrontEndElectronics_INCLUDE_DIRS})

find_package(ROOT)
message(STATUS "Root_INCLUDE_DIR=${ROOT_INCLUDE_DIRS}")
include_directories(${ROOT_INCLUDE_DIRS})

# - Executable:
add_executable(WaveformDrawer
  src/WaveformDrawer.cxx
  )

target_link_libraries(WaveformDrawer
  SNFrontEndElectronics::snfee
  ${ROOT_LIBRARIES}
  )

set_target_properties(WaveformDrawer
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  ) 
	
install(FILES
  ${CMAKE_BINARY_DIR}/WaveformDrawer
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
  PERMISSIONS 
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )
# end
