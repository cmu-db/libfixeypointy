file(GLOB LIBFIXEYPOINTY_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*test.cpp")

################################################################################
# DEPENDENCIES
################################################################################

# valgrind
find_program(VALGRIND_BIN valgrind)
if ("${VALGRIND_BIN}" STREQUAL "VALGRIND_BIN-NOTFOUND")
    message(WARNING "libfixeypointy/test could not find valgrind.")
else()
    message(STATUS "libfixeypointy/test found valgrind at ${VALGRIND_BIN}")
endif()

set(VALGRIND_OPTIONS
        --error-exitcode=1                      # if leaks are detected, return nonzero value
        # --gen-suppressions=all  # uncomment for leak suppression syntax
        --leak-check=full                       # detailed leak information
        --soname-synonyms=somalloc=*jemalloc*   # also intercept jemalloc
        --trace-children=yes                    # trace child processes
        --track-origins=yes                     # track origin of uninitialized values
        )
# set(VALGRIND_SUPPRESSIONS_FILE "${BUILD_SUPPORT_DIR}/valgrind.supp")

################################################################################
# MAKE TARGETS
################################################################################

##########################################
# "make check-tests"
##########################################
add_custom_target(build-tests COMMAND ${CMAKE_CTEST_COMMAND} --show-only)
add_custom_target(check-tests COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

##########################################
# "make XYZ_test"
##########################################
foreach (libfixeypointy_test_source ${LIBFIXEYPOINTY_TEST_SOURCES})
    # Create a human readable name.
    get_filename_component(libfixeypointy_test_filename ${libfixeypointy_test_source} NAME)
    string(REPLACE ".cpp" "" libfixeypointy_test_name ${libfixeypointy_test_filename})

    # Add the test target separately and as part of "make check-tests".
    add_executable(${libfixeypointy_test_name} EXCLUDE_FROM_ALL ${libfixeypointy_test_source})
    add_dependencies(build-tests ${libfixeypointy_test_name})
    add_dependencies(check-tests ${libfixeypointy_test_name})

    target_link_libraries(${libfixeypointy_test_name} libfixeypointy gtest gmock_main)

    # Set test target properties and dependencies.
    set_target_properties(${libfixeypointy_test_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
        COMMAND ${libfixeypointy_test_name}
    )

    # Add the test under CTest.
    add_test(
        ${libfixeypointy_test_name}
        ${VALGRIND_BIN} ${VALGRIND_OPTIONS} --suppressions=${VALGRIND_SUPPRESSIONS_FILE}
        ${CMAKE_BINARY_DIR}/test/${libfixeypointy_test_name}
        --gtest_color=yes
        --gtest_output=xml:${CMAKE_BINARY_DIR}/test/unit_${test_name}.xml
    )
    add_test(
        ${libfixeypointy_test_name}
        ${CMAKE_BINARY_DIR}/test/${libfixeypointy_test_name}
        --gtest_color=yes
        --gtest_output=xml:${CMAKE_BINARY_DIR}/test/${libfixeypointy_test_name}.xml
    )
    
endforeach(libfixeypointy_test_source ${LIBFIXEYPOINTY_TEST_SOURCES})
