cmake_minimum_required(VERSION 3.16)
project(detector C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC_LIST "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADER_LIST "include/*.hpp" "include/*.h")

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS} )

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../common" "${CMAKE_CURRENT_BINARY_DIR}/common")

add_library(${PROJECT_NAME} ${SRC_LIST} ${HEADER_LIST})

target_link_libraries(detector ${OpenCV_LIBS} common Eigen3::Eigen)


