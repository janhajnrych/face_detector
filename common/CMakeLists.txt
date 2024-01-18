cmake_minimum_required(VERSION 3.16)
project(common C CXX)

if(TARGET gui) # cmake include guard
  return()
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC_LIST "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADER_LIST "include/*.hpp" "include/*.h")

add_library(${PROJECT_NAME} ${SRC_LIST} ${HEADER_LIST})

