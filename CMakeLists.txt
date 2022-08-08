cmake_minimum_required(VERSION 3.18)

project(memleak)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

message(STATUS "System name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler ID: ${CMAKE_C_COMPILER_ID}")

add_compile_options(-Wall -Wextra -pedantic -Werror -g
    -fno-omit-frame-pointer
    -fsanitize=address,undefined
    -fno-sanitize-recover=all)
add_link_options(-fsanitize=address)

set(ASAN_OPTIONS "ASAN_OPTIONS=detect_leaks=1:abort_on_error=0")
set(UBSAN_OPTIONS "UBSAN_OPTIONS=print_stacktrace=1")
set(LSAN_OPTIONS "LSAN_OPTIONS=suppressions=${CMAKE_SOURCE_DIR}/lsan.supp")

set(TEST_ENV "${ASAN_OPTIONS};${UBSAN_OPTIONS};${LSAN_OPTIONS}")

macro(add_memleak_test basename)
    add_executable(${basename} ${basename}.c)
    add_test(NAME ${basename} COMMAND ${basename})
    set_tests_properties(${basename} PROPERTIES ENVIRONMENT "${TEST_ENV}")
endmacro()

enable_testing()
include(CTest)

# these should fail if ASAN is used
add_memleak_test(access-after-free)
set_tests_properties(access-after-free PROPERTIES WILL_FAIL TRUE)

add_memleak_test(double-free)
set_tests_properties(double-free PROPERTIES WILL_FAIL TRUE)

# this should fail if UBSAN is used
# adding -fno-sanitize-recover=all is necessary
add_memleak_test(ub-test)
set_tests_properties(ub-test PROPERTIES WILL_FAIL TRUE)

# this should fail if LSAN is used
add_memleak_test(memory-leak)
set_tests_properties(memory-leak PROPERTIES WILL_FAIL TRUE)

# this should not fail
add_memleak_test(no-leak)
