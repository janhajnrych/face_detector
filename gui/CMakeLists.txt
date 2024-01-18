cmake_minimum_required(VERSION 3.16)
project(gui CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Multimedia Charts)

file(GLOB_RECURSE MOC_HEADER_LIST "include/*.hpp" "include/*.h")
file(GLOB_RECURSE SRC_LIST "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADER_LIST "include/*.hpp" "include/*.h")
file(GLOB_RECURSE UI_LIST "ui/*.ui")

set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
SET_PROPERTY(GLOBAL PROPERTY AUTOMOC_FOLDER automoc)


add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../common" "${CMAKE_CURRENT_BINARY_DIR}/common")

add_library(${PROJECT_NAME} ${SRC_LIST} ${HEADER_LIST})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt::Core Qt::Gui Qt5::Widgets Qt5::Multimedia Qt5::Charts common)


