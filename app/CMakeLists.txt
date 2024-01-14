cmake_minimum_required(VERSION 3.16)
project(faceDetectorApp C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../gui"
                 "${CMAKE_CURRENT_BINARY_DIR}/../gui")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../detector"
              "${CMAKE_CURRENT_BINARY_DIR}/../detector")

file(GLOB_RECURSE SRC_LIST "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADER_LIST "include/*.hpp" "include/*.h")

add_executable(${PROJECT_NAME}  ${SRC_LIST} ${HEADER_LIST})
target_link_libraries(${PROJECT_NAME} detector gui)

