cmake_minimum_required(VERSION 3.18)

project(memleak)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

message(STATUS "System name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler ID: ${CMAKE_C_COMPILER_ID}")

# determine if ASan is available
try_compile(ASAN_AVAILABLE "${CMAKE_BINARY_DIR}"
    SOURCES "${CMAKE_SOURCE_DIR}/no-leak.c"
    CMAKE_FLAGS "-fsanitize=address"
    LINK_OPTIONS "-fsanitize=address"
    COPY_FILE "${CMAKE_BINARY_DIR}/lsan-check/no-leak")
message(STATUS "ASan available: ${ASAN_AVAILABLE}")

# determine if LSan is available with ASan
set(LSAN_AVAILABLE FALSE)

if(ASAN_AVAILABLE)
    file(COPY_FILE "${CMAKE_SOURCE_DIR}/lsan-check.sh" "${CMAKE_BINARY_DIR}/lsan-check/lsan-check.sh")
    set(ENV{SOURCE_DIR} ${CMAKE_SOURCE_DIR})

    execute_process(COMMAND
        "${CMAKE_BINARY_DIR}/lsan-check/lsan-check.sh"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/lsan-check"
        RESULT_VARIABLE LSAN_CHECK_RESULT
        OUTPUT_QUIET ERROR_QUIET)

    if(LSAN_CHECK_RESULT STREQUAL "0")
        set(LSAN_AVAILABLE TRUE)
    endif()
endif()

message(STATUS "ASan+LSan available: ${LSAN_AVAILABLE}")

# determine if UBSan is available
try_compile(UBSAN_AVAILABLE "${CMAKE_BINARY_DIR}"
    SOURCES "${CMAKE_SOURCE_DIR}/no-leak.c"
    CMAKE_FLAGS "-fsanitize=undefined"
    LINK_OPTIONS "-fsanitize=undefined")
message(STATUS "UBSan available: ${UBSAN_AVAILABLE}")

# determine if MSan is available
try_compile(MSAN_AVAILABLE "${CMAKE_BINARY_DIR}"
    SOURCES "${CMAKE_SOURCE_DIR}/no-leak.c"
    CMAKE_FLAGS "-fsanitize=memory"
    LINK_OPTIONS "-fsanitize=memory")
message(STATUS "MSan available: ${MSAN_AVAILABLE}")

add_compile_options(-Wall -Wextra -pedantic -Werror -g -fno-omit-frame-pointer)

# add test environment variables
set(TEST_ENV "")

set(USING_ASAN FALSE)
set(USING_LSAN FALSE)
set(USING_UBSAN FALSE)
set(USING_MSAN FALSE)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Asan")
    set(USING_ASAN TRUE)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)

    if(LSAN_AVAILABLE)
        set(USING_LSAN TRUE)
        set(ASAN_OPTIONS "ASAN_OPTIONS=detect_leaks=1:abort_on_error=0")
        set(LSAN_OPTIONS "LSAN_OPTIONS=suppressions=${CMAKE_SOURCE_DIR}/lsan.supp")
        string(CONCAT TEST_ENV "${TEST_ENV}" "${ASAN_OPTIONS};${LSAN_OPTIONS}")
    else()
        string(CONCAT TEST_ENV "${TEST_ENV}" "ASAN_OPTIONS=abort_on_error=0")
    endif()

    if(UBSAN_AVAILABLE)
        # -fno-sanitize-recover=all is necessary for UBSAN test failure
        set(USING_UBSAN TRUE)
        add_compile_options(-fsanitize=undefined -fno-sanitize-recover=all)
        add_link_options(-fsanitize=undefined)
        string(CONCAT TEST_ENV "${TEST_ENV};" "UBSAN_OPTIONS=print_stacktrace=1")
    endif()
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Msan")
    set(USING_MSAN TRUE)
    add_compile_options(-fsanitize=memory -O2)
    add_link_options(-fsanitize=memory)
endif()

macro(add_memleak_test basename)
    add_executable(${basename} ${basename}.c)
    add_test(NAME ${basename} COMMAND ${basename})
    set_tests_properties(${basename} PROPERTIES ENVIRONMENT "${TEST_ENV}")
endmacro()

enable_testing()
include(CTest)

# these should fail if ASan is used
add_memleak_test(access-after-free)
set_tests_properties(access-after-free PROPERTIES WILL_FAIL ${USING_ASAN})

add_memleak_test(double-free)
set_tests_properties(double-free PROPERTIES WILL_FAIL ${USING_ASAN})

# this should fail if LSan is used
add_memleak_test(memory-leak)
set_tests_properties(memory-leak PROPERTIES WILL_FAIL ${USING_LSAN})

# this should fail if UBSan is used
# adding -fno-sanitize-recover=all is necessary
add_memleak_test(ub-test)
set_tests_properties(ub-test PROPERTIES WILL_FAIL ${USING_UBSAN})

add_memleak_test(umr)
set_tests_properties(umr PROPERTIES WILL_FAIL ${USING_MSAN})

# this should not fail
add_memleak_test(no-leak)
