cmake_minimum_required (VERSION 3.0)
project (libtwsclient)

# Point CMake at any custom modules we may ship
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

find_package(LibUV REQUIRED)
include_directories(SYSTEM ${LIBUV_INCLUDE_DIRS})

find_package(Jemalloc)
include_directories(SYSTEM ${JEMALLOC_INCLUDE_DIRS})

find_package(Zlog)
include_directories(SYSTEM ${ZLOG_INCLUDE_DIRS})

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/include)

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/include/msvc_compat)
endif()

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -std=gnu99" )
endif()

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

add_subdirectory(src)
add_subdirectory(test)

