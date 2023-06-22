cmake_minimum_required(VERSION 3.26.0)
project(cpm VERSION 0.0.1)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_STANDARD 17)

file(GLOB_RECURSE CLI_SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/cli/*.c")
file(GLOB_RECURSE CLI_HDR_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/cli/*.h")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")

add_executable(cli ${CLI_SRC_FILES} ${CLI_HDR_FILES})
include_directories(cli PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/cli")

add_compile_definitions(PROJECT="${CMAKE_PROJECT_NAME}")
add_compile_definitions(VERSION="${CMAKE_PROJECT_VERSION}")
add_compile_definitions(PROJECT_REPO="https://github.com/resyfer/cpm")
add_compile_definitions(PROJECT_ISSUES="https://github.com/resyfer/cpm/issues")

add_custom_target(
  format ALL
  COMMAND cd .. && indent ${CLI_SRC_FILES} ${CLI_HDR_FILES} && cd ${CMAKE_BINARY_DIR} && rm -f "${CMAKE_CURRENT_SOURCE_DIR}/**/**/*.{c,h}~*"
)
add_dependencies(cli format)