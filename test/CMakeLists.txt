cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)

# ---- Test as standalone project the exported config package ----

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
  enable_testing()

  find_package(${PROJECT_NAME} CONFIG REQUIRED) # for intro, project_options,
                                                # ...
  if(NOT TARGET options)
    message(FATAL_ERROR "Required config package not found!")
    return() # be strictly paranoid for Template Janitor github action! CK
  endif()
endif()

# ---- Dependencies ----

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE knn::warnings knn::options
                                    Catch2::Catch2WithMain)

# automatically discover tests that are defined in catch based test files you
# can modify the unittests. Set TEST_PREFIX to whatever you want, or use
# different for different binaries
catch_discover_tests(
  tests
  TEST_PREFIX
  "unittests."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)
