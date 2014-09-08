set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB TESTS_SOURCES *.c)

include_directories(${PROJECT_SOURCE_DIR}/src)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_executable(twsdemo ${TESTS_SOURCES})
target_link_libraries(twsdemo twsclient)

install(TARGETS twsdemo
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
