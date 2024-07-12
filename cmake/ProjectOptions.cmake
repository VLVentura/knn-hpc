include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)

macro(supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES
                                                   ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES
                                                   ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(setup_options)
  option(ENABLE_HARDENING "Enable hardening" ON)
  option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies" ON
    ENABLE_HARDENING OFF)

  supports_sanitizers()

  if(ENABLE_DEVELOPER_MODE)
    option(ENABLE_IPO "Enable IPO/LTO" ON)
    option(WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(ENABLE_SANITIZER_LEAK "Enable leak sanitizer" ON)
    option(ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer"
           ${SUPPORTS_UBSAN})
    option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" ON)
    option(ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(ENABLE_PCH "Enable precompiled headers" OFF)
    option(ENABLE_CACHE "Enable ccache" ON)
  else()
    option(ENABLE_IPO "Enable IPO/LTO" OFF)
    option(WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(ENABLE_PCH "Enable precompiled headers" OFF)
    option(ENABLE_CACHE "Enable ccache" OFF)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      ENABLE_IPO
      WARNINGS_AS_ERRORS
      ENABLE_USER_LINKER
      ENABLE_SANITIZER_ADDRESS
      ENABLE_SANITIZER_LEAK
      ENABLE_SANITIZER_UNDEFINED
      ENABLE_SANITIZER_THREAD
      ENABLE_SANITIZER_MEMORY
      ENABLE_UNITY_BUILD
      ENABLE_CLANG_TIDY
      ENABLE_CPPCHECK
      ENABLE_COVERAGE
      ENABLE_PCH
      ENABLE_CACHE)
  endif()

  check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED
     AND (ENABLE_SANITIZER_ADDRESS
          OR ENABLE_SANITIZER_THREAD
          OR ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(global_options)
  if(ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    enable_ipo()
  endif()

  supports_sanitizers()

  if(ENABLE_HARDENING AND ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN
       OR ENABLE_SANITIZER_UNDEFINED
       OR ENABLE_SANITIZER_ADDRESS
       OR ENABLE_SANITIZER_THREAD
       OR ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message(
      "${ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${ENABLE_SANITIZER_UNDEFINED}"
    )
    enable_hardening(options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(warnings INTERFACE)
  add_library(options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  set_project_warnings(warnings ${WARNINGS_AS_ERRORS} "" "" "" "")

  if(ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    configure_linker(options)
  endif()

  include(cmake/Sanitizers.cmake)
  enable_sanitizers(
    options ${ENABLE_SANITIZER_ADDRESS} ${ENABLE_SANITIZER_LEAK}
    ${ENABLE_SANITIZER_UNDEFINED} ${ENABLE_SANITIZER_THREAD}
    ${ENABLE_SANITIZER_MEMORY})

  set_target_properties(options PROPERTIES UNITY_BUILD ${ENABLE_UNITY_BUILD})

  if(ENABLE_PCH)
    target_precompile_headers(options INTERFACE <vector> <string> <utility>)
  endif()

  if(ENABLE_CACHE)
    include(cmake/Cache.cmake)
    enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(ENABLE_CLANG_TIDY)
    enable_clang_tidy(options ${WARNINGS_AS_ERRORS})
  endif()

  if(ENABLE_CPPCHECK)
    enable_cppcheck(${WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    enable_coverage(options)
  endif()

  if(WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(ENABLE_HARDENING AND NOT ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN
       OR ENABLE_SANITIZER_UNDEFINED
       OR ENABLE_SANITIZER_ADDRESS
       OR ENABLE_SANITIZER_THREAD
       OR ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    enable_hardening(options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
