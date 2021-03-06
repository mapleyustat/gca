cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(gca)

# Makes sure that executables go to bin directory (respective to build dir)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
SET(MY_OWN_INSTALL_PREFIX "bin" CACHE PATH "Prefix prepended to install directories")
SET(CMAKE_INSTALL_PREFIX "${MY_OWN_INSTALL_PREFIX}" CACHE INTERNAL "Prefix prepended to install directories" FORCE)

# If Eigen found EIGEN_ENABLED define to compile flags
find_package(Eigen)
if(EIGEN_FOUND)
  include_directories(SYSTEM ${EIGEN_INCLUDE_DIRS})
  add_definitions(-DEIGEN_ENABLED)
endif(EIGEN_FOUND)

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++0x -O3")

# add a target to generate API documentation with Doxygen
#find_package(Doxygen)
#if(DOXYGEN_FOUND)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
#add_custom_target(doc
#${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
#WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#COMMENT "Generating API documentation with Doxygen" VERBATIM
#)
#endif(DOXYGEN_FOUND)

# add the endoscopy src for compile - it has its own cmake config
add_subdirectory(tests)
