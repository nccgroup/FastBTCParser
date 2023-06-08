# Released as open source by NCC Group Plc - https://www.nccgroup.com/
# https://github.com/nccgroup/FastBTCParser

cmake_minimum_required(VERSION 3.22)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(fastbtcparser)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

FILE(GLOB_RECURSE HeadFiles "./*.h")
add_custom_target(headers SOURCES ${HeadFiles})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ssl crypto pthread ncurses)