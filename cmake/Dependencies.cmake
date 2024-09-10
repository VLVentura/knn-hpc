include(cmake/CPM.cmake)

# Done as a function so that updates to variables like CMAKE_CXX_FLAGS don't
# propagate out to other targets
function(setup_dependencies)
  cmake_policy(SET CMP0169 OLD)

  CPMAddPackage("gh:fmtlib/fmt#11.0.1")

  CPMAddPackage(
    NAME spdlog
    VERSION 1.14.0
    GITHUB_REPOSITORY "gabime/spdlog"
    OPTIONS "SPDLOG_FMT_EXTERNAL ON"
  )

  CPMAddPackage(
    NAME Eigen
    VERSION 3.4
    URL https://gitlab.com/libeigen/eigen/-/archive/3.4/eigen-3.4.tar.gz
    # Eigen's CMakelists are not intended for library use
    DOWNLOAD_ONLY YES
  )
  if(Eigen_ADDED)
    add_library(Eigen INTERFACE IMPORTED)
    target_include_directories(Eigen INTERFACE ${Eigen_SOURCE_DIR})
  endif()

  CPMAddPackage("gh:CLIUtils/CLI11@2.4.2")

  CPMAddPackage("gh:catchorg/Catch2@3.3.2")

  find_package(OpenMP REQUIRED)
  message(STATUS "OpenMP Version: ${OpenMP_CXX_VERSION}")

  find_package(MPI REQUIRED)
  message(STATUS "MPI C++ Compiler: ${MPI_CXX_COMPILER}")
  message(STATUS "MPI Executable: ${MPIEXEC_EXECUTABLE}")

endfunction()
