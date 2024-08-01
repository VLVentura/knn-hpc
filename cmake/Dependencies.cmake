include(cmake/CPM.cmake)

# Done as a function so that updates to variables like CMAKE_CXX_FLAGS don't
# propagate out to other targets
function(setup_dependencies)

  CPMAddPackage("gh:fmtlib/fmt#11.0.1")

  CPMAddPackage(
    NAME spdlog
    VERSION 1.14.0
    GITHUB_REPOSITORY "gabime/spdlog"
    OPTIONS "SPDLOG_FMT_EXTERNAL ON"
  )

  CPMAddPackage("gh:CLIUtils/CLI11@2.4.2")

  CPMAddPackage("gh:catchorg/Catch2@3.3.2")

endfunction()
