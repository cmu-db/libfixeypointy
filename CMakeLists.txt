cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)              # Compile as C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # Require C++17 support
set(BUILD_SHARED_LIBS ON)

project(libfixeypointy
        VERSION 1.0
        DESCRIPTION "Fixed-Point Decimal Library"
        LANGUAGES CXX
)

# Expected directory structure.
set(BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/support")

################################################################################
# DEPENDENCIES
################################################################################

# CTest
enable_testing()

# clang-format
find_program(CLANG_FORMAT_BIN
        NAMES clang-format clang-format-8
        HINTS ${LIBFIXEYPOINTY_CLANG_SEARCH_PATH})
if ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
    message(WARNING "libfixeypointy/main could not find clang-format.")
else()
    message(STATUS "libfixeypointy/main found clang-format at ${CLANG_FORMAT_BIN}")
endif()

# gmock gtest https://crascit.com/2015/07/25/cmake-gtest/
configure_file(
        "${BUILD_SUPPORT_DIR}/gtest_CMakeLists.txt.in"
        googletest-download/CMakeLists.txt
)
execute_process(
        COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
)
execute_process(
        COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
)
# don't override our compiler/linker options when building gtest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  
add_subdirectory(
        "${CMAKE_BINARY_DIR}/googletest-src"
        "${CMAKE_BINARY_DIR}/googletest-build"
)

################################################################################
# COMPILER SETUP
################################################################################

# Compiler flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -Werror -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")

set(BUSTUB_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src)
include_directories(
        ${LIBFIXEYPOINTY_SRC_INCLUDE_DIR}
)
include_directories(BEFORE src) # This is needed for gtest.

add_subdirectory(test)

################################################################################
# MAKE TARGETS
################################################################################

# clang-format
# runs clang format and updates files in place.
string(CONCAT LIBFIXEYPOINTY_FORMAT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_custom_target(format ${BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs ${LIBFIXEYPOINTY_FORMAT_DIRS}
        --fix
        --quiet
)
# Run clang-format and exit with a non-zero exit code if any files need to be reformatted.
add_custom_target(check-format
        ${BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs ${LIBFIXEYPOINTY_FORMAT_DIRS}
        --quiet
        USES_TERMINAL
)

# Shared Library
add_library(libfixeypointy SHARED src/decimal.cpp src/magic_numbers.h src/custom_magic_numbers.h)

# Demo Program
add_executable(demo src/demo.cpp)
target_link_libraries(demo libfixeypointy)
