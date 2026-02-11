cmake_minimum_required(VERSION 3.16)

project(mission_data_service
    VERSION 1.0
    DESCRIPTION "Containerized C++ Mission Data Service"
    LANGUAGES CXX
)

# Enforce C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Strict compiler warnings (defense-style discipline)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
    )
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/http_server.cpp
    src/validator.cpp
    src/data_store.cpp
)

set(HEADERS
    src/http_server.h
    src/validator.h
    src/data_store.h
    src/data_record.h
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/external
)

# Optional: enable sanitizers in Debug mode
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined)
        target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined)
    endif()
endif()

# Installation target
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin)
