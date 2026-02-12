cmake_minimum_required(VERSION 3.16)

project(mission_data_service
    VERSION 1.0
    DESCRIPTION "Containerized C++ Mission Data Service"
    LANGUAGES CXX
)

# ------------------------------------------------------------
# C++ Standard
# ------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------
# Strict Compiler Warnings (Defense-style discipline)
# ------------------------------------------------------------
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
elseif (MSVC)
    add_compile_options(/W4 /permissive-)
endif()

# ------------------------------------------------------------
# Source Files
# ------------------------------------------------------------
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

# ------------------------------------------------------------
# Main Executable
# ------------------------------------------------------------
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/external
)

# ------------------------------------------------------------
# Sanitizers
# ------------------------------------------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined)
        target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined)
    endif()
endif()

# ------------------------------------------------------------
# Gtest Setup
# ------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# For Windows: prevent overriding runtime settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

# ------------------------------------------------------------
# Test Sources
# ------------------------------------------------------------
set(TEST_SOURCES
    tests/test_validator.cpp
    tests/test_data_store.cpp
)

add_executable(mission_data_service_tests
    ${TEST_SOURCES}
    src/validator.cpp
    src/data_store.cpp
)

target_include_directories(mission_data_service_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(mission_data_service_tests
    PRIVATE
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(mission_data_service_tests)

# ------------------------------------------------------------
# Install Target
# ------------------------------------------------------------
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin)
