cmake_minimum_required(VERSION 3.16)
project(detector C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC_LIST "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADER_LIST "include/*.hpp" "include/*.h")

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package( Boost REQUIRED)
include_directories( ${Boost_INCLUDE_DIRS} )

add_library(${PROJECT_NAME} ${SRC_LIST} ${HEADER_LIST})
target_link_libraries(detector ${OpenCV_LIBS}) #tensorflow-lite

