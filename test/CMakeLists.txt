cmake_minimum_required(VERSION 3.16)
project(test C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# LIST(APPEND CMAKE_PROGRAM_PATH  "c:/MyProject/Tools/mingw/bin/" ...)
# export LD_LIBRARY_PATH=/opt/openssl-1.1.1d/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
# /opt/openssl-1.1.1d/bin/openssl version


file(GLOB_RECURSE SRC_LIST "src/*.cpp" "src/*.c" "src/*.cc")
file(GLOB_RECURSE HEADER_LIST "include/*.hpp" "include/*.h")

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} ${gtest_SOURCE_DIR}/include)
find_package(GTest REQUIRED)


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/img1.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/img2.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/img3.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/img4.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/img5.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/img6.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)

add_executable(${PROJECT_NAME}  ${SRC_LIST} ${HEADER_LIST})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} gtest gtest_main detector)

enable_testing()







