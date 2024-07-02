cmake_minimum_required(VERSION 3.6)
project(ghud)
option(DEBUG_BUILD "Option description" OFF)
set(CMAKE_CXX_STANDARD 17)
set(BIN_DIR ${CMAKE_BINARY_DIR}/Binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR})
set(PROJECT_NAME ghud)

message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
if(DEBUG_BUILD)
    message(STATUS "DEBUG_BUILD = ON")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fno-omit-frame-pointer -fasynchronous-unwind-tables")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fno-omit-frame-pointer -fasynchronous-unwind-tables")
else()
    message(STATUS "DEBUG_BUILD = OFF")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Os -s")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")
    #set(CMAKE_INSTALL_RPATH "/")
endif()

set(Sources
        main.cpp
        curl_request.cpp
        gitapi_request.cpp
        ghud.cpp
        ghud_repo.cpp
        )

message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")

add_executable(${PROJECT_NAME} ${Sources})

target_link_libraries(${PROJECT_NAME} curl)
target_link_libraries(${PROJECT_NAME} mxml)
