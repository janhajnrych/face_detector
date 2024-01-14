cmake_minimum_required(VERSION 3.16)
project(faceDetector C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/app"
            "${CMAKE_CURRENT_BINARY_DIR}/app")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test"
            "${CMAKE_CURRENT_BINARY_DIR}/test")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/haarcascade_frontalface_default.xml
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)

