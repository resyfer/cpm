cmake_minimum_required(VERSION 3.26.0)
project(cpm VERSION 0.0.1)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_STANDARD 17)

file(GLOB_RECURSE CLI_SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/cli/*.c")
file(GLOB_RECURSE CLI_HDR_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/cli/*.h")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")

add_executable(cli ${CLI_SRC_FILES} ${CLI_HDR_FILES})
include_directories(cli PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/cli")