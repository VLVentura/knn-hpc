file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:DEBUG>
                                                  $<$<CONFIG:Release>:NDEBUG>)

target_link_libraries(${PROJECT_NAME} PRIVATE options warnings)

target_link_system_libraries(${PROJECT_NAME} PRIVATE fmt::fmt spdlog::spdlog
                             CLI11::CLI11)
