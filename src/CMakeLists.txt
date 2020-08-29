if(WIN32)

file(GLOB_RECURSE ALL_HEADERS
    ${PROJECT_SOURCE_DIR}/include/**/*.h
    ${PROJECT_SOURCE_DIR}/include/**/*.hpp
    ${PROJECT_SOURCE_DIR}/src/**/*.h
    ${PROJECT_SOURCE_DIR}/src/**/*.hpp
    )

endif(WIN32)

set(${PROJECT_NAME}_source_files
    ${ALL_HEADERS}

    binaryparser/BinaryParser.cpp
    binaryparser/BinaryParserConfigurator.cpp

    datareceiver/TCPReceiver.cpp
    datareceiver/UDPReceiver.cpp
    
    datastorage/StorageTask.cpp
    datastorage/StorageConverter.cpp

    DataStorage.cpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_source_files})
target_include_directories(${PROJECT_NAME} PRIVATE ${THIRDPARTY_INCLUDE_DIR} ${HEADER_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PRIVATE ${THIRDPARTY_LINK_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${THIRDPARTY_LIBRARIES})

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION ${BIN_INSTALL_DIR})