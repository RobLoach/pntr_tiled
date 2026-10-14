cmake_minimum_required(VERSION 3.14)
project(pntr_tiled
    DESCRIPTION "pntr_tiled"
    HOMEPAGE_URL "https://github.com/RobLoach/pntr_tiled"
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
option(PNTR_TILED_WERROR "pntr_tiled: Treat warnings as errors in the tests and examples" OFF)

# Apply pntr_tiled's warning flags to one of its own targets. These are deliberately not put on
# the pntr_tiled interface library, so that consumers are never forced into them.
function(pntr_tiled_set_warnings target)
    if (MSVC)
        target_compile_options(${target} PRIVATE /W4)
        if (PNTR_TILED_WERROR)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wmissing-prototypes -pedantic)
        if (PNTR_TILED_WERROR)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()

    # The dependencies are vendored third party headers, so their own warnings are not
    # pntr_tiled's to fix.
    foreach (dependency pntr pntr_app pntr_assetsys)
        if (TARGET ${dependency})
            get_target_property(dependency_includes ${dependency} INTERFACE_INCLUDE_DIRECTORIES)
            if (dependency_includes)
                set_property(TARGET ${dependency} APPEND PROPERTY
                    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${dependency_includes}
                )
            endif()
        endif()
    endforeach()
endfunction()

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
