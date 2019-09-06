cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(TimeDistrib LANGUAGES CXX)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "${PROJECT_NAME} requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

find_package(SNFrontEndElectronics)
message(STATUS  "SNFrontEndElectronics_INCLUDE_DIR=${SNFrontEndElectronics_INCLUDE_DIR}")
include_directories(${SNFrontEndElectronics_INCLUDE_DIRS})

find_package(ROOT)
message(STATUS "Root_INCLUDE_DIR=${ROOT_INCLUDE_DIRS}")
include_directories(${ROOT_INCLUDE_DIRS})

# - Executable:
add_executable(TimeDistrib
  src/TimeDistrib.cxx
  )

target_link_libraries(TimeDistrib
  SNFrontEndElectronics::snfee
  ${ROOT_LIBRARIES}
  )

set_target_properties(TimeDistrib
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  )

install(FILES
  ${CMAKE_BINARY_DIR}/TimeDistrib
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
  PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )
# end
