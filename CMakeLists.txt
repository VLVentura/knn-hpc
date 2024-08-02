cmake_minimum_required(VERSION 3.21)

# C compiler
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS ON)

# C++ compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS ON)

# Global cache variables
set(ENABLE_DEVELOPER_MODE OFF CACHE BOOL "Enable developer mode")
message(STATUS "Developer mode enabled: ${ENABLE_DEVELOPER_MODE}")

# Set the project name and language
project(
  knn
  VERSION 0.0.1
  DESCRIPTION "KNN Algorithm implemented with HPC techniques"
  HOMEPAGE_URL "https://github.com/VLVentura/knn-hpc"
  LANGUAGES CXX C
)

# Optimize code for process architecture
# include(cmake/OptimizeForArchitecture.cmake)
# OptimizeForArchitecture()

# Setup project configurations
include(cmake/PreventInSourceBuilds.cmake)
include(cmake/ProjectOptions.cmake)
setup_options()
global_options()

# Adding third_parties
include(cmake/Dependencies.cmake)
setup_dependencies()
local_options()

# Con't know if this should be set globally from here or not...
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(GIT_SHA "Unknown" CACHE STRING "SHA this build was generated from")
string(SUBSTRING "${GIT_SHA}" 0 8 GIT_SHORT_SHA)

target_compile_features(options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

add_library(knn::options ALIAS options)
add_library(knn::warnings ALIAS warnings)

# Configure files based on CMake configuration options
add_subdirectory(configured_files)

# Adding the src:
include_directories(src)
add_subdirectory(src)

# Adding the tests: if(ENABLE_TESTING) include(CTest) add_subdirectory(test)
# endif()
