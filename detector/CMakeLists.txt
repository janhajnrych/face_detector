# include(GNUInstallDirs)
# install(TARGETS nn
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

cmake_minimum_required(VERSION 3.16)
project(detector C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(TENSORFLOW_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../tensorflow_src" CACHE PATH
#   "Directory that contains the TensorFlow project" )
# add_subdirectory(
#   "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite"
#   "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite" EXCLUDE_FROM_ALL)

file(GLOB_RECURSE SRC_LIST "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADER_LIST "include/*.hpp" "include/*.h")

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_library(detector ${SRC_LIST} ${HEADER_LIST})
target_link_libraries(detector ${OpenCV_LIBS}) #tensorflow-lite





# set(TENSORFLOW_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../tensorflow_src" CACHE PATH
#   "Directory that contains the TensorFlow project" )
# add_subdirectory(
#   "${TENSORFLOW_SOURCE_DIR}/../tensorflow/lite"
#   "${CMAKE_CURRENT_BINARY_DIR}/../tensorflow-lite" EXCLUDE_FROM_ALL)

# file(GLOB_RECURSE SRC_LIST "src/*.cpp" "src/*.c")
# file(GLOB_RECURSE HEADER_LIST "include/*.hpp" "include/*.h")

# add_library(detector ${SRC_LIST} ${HEADER_LIST})
# target_link_libraries(detector tensorflow-lite)

