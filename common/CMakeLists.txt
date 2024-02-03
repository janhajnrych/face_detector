cmake_minimum_required(VERSION 3.16)
project(common C CXX)

if(TARGET gui) # cmake include guard
  return()
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VERSION "0.0.2")
set(IS_DEBUG 0)
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(IS_DEBUG 1)
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")


file(WRITE include/version.h
     "#ifndef VERSION_H\n"
     "#define VERSION_H\n\n"
     "#define IS_DEBUG ${IS_DEBUG}\n"
     "namespace BuildInfo {\n\n"
     "  const char* getVersion();\n"
     "  const char* getBuildType();\n"
     "  inline bool isDebug() {return ${IS_DEBUG};}\n\n"
     "}\n\n#endif\n"
)

file(WRITE src/version.cpp
     "#include \"../include/version.h\"\n\n"
     "namespace BuildInfo {\n\n"
     "  const char* getVersion() { return \"${VERSION}\"; }\n"
     "  const char* getBuildType() { return \"${CMAKE_BUILD_TYPE}\"; }\n\n"
     "}"
)


file(GLOB_RECURSE SRC_LIST "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADER_LIST "include/*.hpp" "include/*.h")

add_library(${PROJECT_NAME} ${SRC_LIST} ${HEADER_LIST})



