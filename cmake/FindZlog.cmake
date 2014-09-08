# - Try to find zlog
# Once done this will define
#  ZLOG_FOUND - System has ZLOG
#  ZLOG_INCLUDE_DIRS - The ZLOG include directories
#  ZLOG_LIBRARIES - The libraries needed to use ZLOG

find_package(PkgConfig)
if(NOT ZLOG_USE_BUNDLED)
  find_package(PkgConfig)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(PC_ZLOG QUIET ZLOG)
  endif()
else()
  set(PC_ZLOG_INCLUDEDIR)
  set(PC_ZLOG_INCLUDE_DIRS)
  set(PC_ZLOG_LIBDIR)
  set(PC_ZLOG_LIBRARY_DIRS)
  set(LIMIT_SEARCH NO_DEFAULT_PATH)
endif()

set(ZLOG_DEFINITIONS ${PC_ZLOG_CFLAGS_OTHER})

find_path(ZLOG_INCLUDE_DIR zlog.h
          PATHS ${PC_ZLOG_INCLUDEDIR} ${PC_ZLOG_INCLUDE_DIRS}
          ${LIMIT_SEARCH})

# If we're asked to use static linkage, add zlog.a as a preferred library name.
if(ZLOG_USE_STATIC)
  list(APPEND ZLOG_NAMES
    "${CMAKE_STATIC_LIBRARY_PREFIX}zlog${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

list(APPEND ZLOG_NAMES ZLOG)

find_library(ZLOG_LIBRARY NAMES ZLOG
             PATHS ${PC_ZLOG_LIBDIR} ${PC_ZLOG_LIBRARY_DIRS}
             ${LIMIT_SEARCH})

set(ZLOG_LIBRARIES ${ZLOG_LIBRARY})
set(ZLOG_INCLUDE_DIRS ${ZLOG_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ZLOG_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(ZLOG DEFAULT_MSG
                                  ZLOG_LIBRARY ZLOG_INCLUDE_DIR)

mark_as_advanced(ZLOG_INCLUDE_DIR ZLOG_LIBRARY)
