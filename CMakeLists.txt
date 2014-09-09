cmake_minimum_required (VERSION 3.0)

project (libtwsclient C)

set(LIBTWSCLIENT_MAJOR 0)
set(LIBTWSCLIENT_MINOR 0)
set(LIBTWSCLIENT_MICRO 1)
set(LIBTWSCLIENT_PATCH 0)

set(LIBTWSCLIENT_VERSION "${LIBTWSCLIENT_MAJOR}.${LIBTWSCLIENT_MINOR}.${LIBTWSCLIENT_MICRO}")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

IF(WIN32)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
  add_definitions(-DWIN32)
  set(linkFlags "/PDB:NONE /INCREMENTAL:NO /OPT:NOREF /OPT:NOICF")
  set(CMAKE_EXE_LINKER_FLAGS    "${linkFlags}"
      CACHE STRING "Libtwsclient linker flags" FORCE)
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG ""
      CACHE STRING "Libtwsclient debug linker flags" FORCE)
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE
      CACHE STRING "Libtwsclient release linker flags" FORCE)
  set(CMAKE_SHARED_LINKER_FLAGS "${linkFlags}"
      CACHE STRING "Libtwsclient shared linker flags" FORCE)
  set(CMAKE_MODULE_LINKER_FLAGS "${linkFlags}"
      CACHE STRING "Libtwsclient module linker flags" FORCE)

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4996 /wd4255 /wd4130 /wd4100 /wd4711")
  set(CMAKE_C_FLAGS_DEBUG "/D DEBUG /Od /Z7")
  set(CMAKE_C_FLAGS_RELEASE "/D NDEBUG /O2")
else (WIN32)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
  if(CMAKE_COMPILER_IS_GNUCC)
    include(CheckCCompilerFlag)
    check_c_compiler_flag(-fvisibility=hidden HAVE_GCC_VISIBILITY)
    if(HAVE_GCC_VISIBILITY)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
    endif(HAVE_GCC_VISIBILITY)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 ")
  else(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 ")
  endif(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS
      "${CMAKE_C_FLAGS} -Wpointer-arith -Wno-format-y2k -Wnested-externs -Wextra	-Wundef -Wwrite-strings -Wold-style-definition -Wredundant-decls -Wno-unused-parameter -Wno-sign-compare")

  set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -g")
  set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O2 -Wuninitialized")
endif (WIN32)

# Point CMake at any custom modules we may ship
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# options
option(BUILD_DEMOS "Build the libtwsclient example programs" ON)

find_package(LibUV REQUIRED)
include_directories(SYSTEM ${LIBUV_INCLUDE_DIRS})

find_package(Jemalloc)
include_directories(SYSTEM ${JEMALLOC_INCLUDE_DIRS})

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/include)

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/include/msvc_compat)
endif()

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -std=gnu99" )
endif()

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

add_subdirectory(src)

if(BUILD_DEMOS)
  add_subdirectory(test)
endif(BUILD_DEMOS)
