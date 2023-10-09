cmake_minimum_required(VERSION 3.11)
project(pntr_tiled
    DESCRIPTION "pntr_tiled"
    HOMEPAGE_URL "https://github.com/robloach/pntr_tiled"
    VERSION 0.1.0
    LANGUAGES C
)

# pntr_tiled
add_library(pntr_tiled INTERFACE)
target_include_directories(pntr_tiled INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Options
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(PNTR_TILED_IS_MAIN TRUE)
else()
    set(PNTR_TILED_IS_MAIN FALSE)
endif()

option(PNTR_TILED_BUILD_EXAMPLES "pntr_tiled: Examples" ${PNTR_TILED_IS_MAIN})
option(PNTR_TILED_BUILD_EXAMPLES_RAYLIB "pntr_tiled: raylib example" ${PNTR_TILED_IS_MAIN})
option(PNTR_TILED_BUILD_TESTS "pntr_tiled: Tests" ${PNTR_TILED_IS_MAIN})

if (PNTR_TILED_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Testing
if (PNTR_TILED_BUILD_TESTS)
    include(CTest)
    enable_testing()
    if (BUILD_TESTING)
        set(CTEST_CUSTOM_TESTS_IGNORE
            pkg-config--static
        )
        # Always print verbose output when tests fail if run using `make test`.
        list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
        add_subdirectory(test)
    endif()
endif()
