cmake_minimum_required(VERSION 3.18)

project(memleak)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

message(STATUS "System name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler ID: ${CMAKE_C_COMPILER_ID}")

set(ASAN True)

if(MSVC)
    add_compile_options(/guard:cf /analyze /permissive- /W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

if(ASAN)

    # Windows compiler
    if(MSVC)
        # add ASAN flag https://docs.microsoft.com/en-us/cpp/sanitizers/asan
        add_compile_options(/fsanitize=address)
        set(USING_ASAN True)
        set(USING_LSAN False)

    # macOS
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        # assume Homebrew Clang and has ASAN and LSAN
        if(("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang"))
            add_compile_options(-fsanitize=address)
            add_link_options(-fsanitize=address)
            set(USING_ASAN True)
            set(USING_LSAN True)

        # Apple Clang has ASAN but not LSAN
        elseif(("${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang"))
            add_compile_options(-fsanitize=address)
            add_link_options(-fsanitize=address)
            set(USING_ASAN True)
            set(USING_LSAN False)

        # assume everything else has neither
        else()
            set(USING_ASAN False)
            set(USING_LSAN False)
        endif()

    # Linux
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
        set(USING_ASAN True)
        set(USING_LSAN True)

    # unknown
    else()
        set(USING_ASAN False)
        set(USING_LSAN False)

    endif()

# cannot use leaks with ASAN
elseif(("${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang"))

    find_program(LEAKS_PATH NAMES leaks)

    if(LEAKS_PATH)
        message(STATUS "leaks command found: ${LEAKS}")
    endif()

endif()

if(USING_ASAN)
    message(STATUS "Using ASAN")
endif()

if(USING_LSAN)
    message(STATUS "Using LSAN")
endif()

macro(add_memleak_test basename)

    add_executable(${basename} ${basename}.c)

    # leaks makes ctests hang for some reason
    # if(LEAKS_PATH)
        # add_test(NAME ${basename} COMMAND leaks --quiet --atExit -- $<TARGET_FILE:${basename}>)
    # elseif(ASAN)
    if(USING_ASAN)

        add_test(NAME ${basename} COMMAND ${basename})

        # detect leaks when using LSAN
        if(USING_LSAN)
            set_tests_properties(${basename} PROPERTIES ENVIRONMENT
            "ASAN_OPTIONS=detect_leaks=1:abort_on_error=0;LSAN_OPTIONS=suppressions=${CMAKE_SOURCE_DIR}/lsan.supp")
        else()
            set_tests_properties(${basename} PROPERTIES ENVIRONMENT
                "ASAN_OPTIONS=abort_on_error=0")
        endif()

    else()

        add_test(NAME ${basename} COMMAND ${basename})

    endif()

endmacro()

enable_testing()

# these should fail if using ASAN
add_memleak_test(access-after-free)
add_memleak_test(double-free)

if(USING_ASAN)
    set_tests_properties(access-after-free PROPERTIES WILL_FAIL True)
    set_tests_properties(double-free PROPERTIES WILL_FAIL True)
endif()

# this should fail if LSAN or leaks is used
add_memleak_test(memory-leak)
if(USING_LSAN OR LEAKS_PATH)
    set_tests_properties(memory-leak PROPERTIES WILL_FAIL True)
endif()

# this should not fail at all
add_memleak_test(no-leak)
