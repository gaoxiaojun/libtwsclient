set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB LIB_TWS_CLIENT_SOURCE *.c)

add_library(twsclient SHARED ${LIB_TWS_CLIENT_SOURCE})
add_library(twsclient_static STATIC ${LIB_TWS_CLIENT_SOURCE})

target_link_libraries(twsclient ${LIBUV_LIBRARIES} ${JEMALLOC_LIBRARIES} ${ZLOG_LIBRARIES})
target_link_libraries(twsclient_static ${LIBUV_LIBRARIES} ${JEMALLOC_LIBRARIES} ${ZLOG_LIBRARIES})

set_target_properties(twsclient PROPERTIES VERSION 0.0.1 SOVERSION 0)
set_target_properties(twsclient PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(twsclient_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(twsclient_static PROPERTIES OUTPUT_NAME "twsclient")

install(TARGETS twsclient twsclient_static
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
