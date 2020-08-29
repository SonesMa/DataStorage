cmake_minimum_required(VERSION 3.8)

project(DataStorage)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

set(CMAKE_CXX_STANDARD 11)

set(THIRDPARTY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/arduinojson
    ${CMAKE_SOURCE_DIR}/thirdparty/csvparser)

set(HEADER_INCLUDE_DIR
    ${CMAKE_SOURCE_DIR}/include/binaryparser
    ${CMAKE_SOURCE_DIR}/include/common
    ${CMAKE_SOURCE_DIR}/include/datareceiver
    ${CMAKE_SOURCE_DIR}/include/datastorage)

add_subdirectory(src)