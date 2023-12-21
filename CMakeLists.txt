cmake_minimum_required(VERSION 3.16)
project(faceDetector C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/gui"
                 "${CMAKE_CURRENT_BINARY_DIR}/gui")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/detector"
              "${CMAKE_CURRENT_BINARY_DIR}/detector")

add_executable(faceDetector main.cpp)
target_link_libraries(faceDetector detector gui)

