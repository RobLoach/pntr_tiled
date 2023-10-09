# CMAKE Modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

# Dependencies
find_package(pntr REQUIRED)
find_package(pntr_app REQUIRED)

# Iterate through each .c file
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(APPEND example_sources ${sources})

# Do each example
foreach (example_source ${example_sources})
    # Create the basename for the example
    get_filename_component(example_name ${example_source} NAME)

    set(example_source_source ${example_source})
    string(REPLACE ".c" "" example_name ${example_name})

    # raylib
    if (PNTR_TILED_BUILD_EXAMPLES_RAYLIB)
        find_package(raylib QUIET)
        set(example_name_raylib ${example_name}_raylib)

        add_executable(${example_name_raylib}
            ${example_source_source}
        )
        target_link_libraries(${example_name_raylib} PUBLIC
            raylib
            pntr
            pntr_app
            pntr_tiled
        )
        target_compile_definitions(${example_name_raylib} PUBLIC PNTR_APP_RAYLIB)
    endif()
endforeach ()

# Resources
file(GLOB resources resources/*)
set(examples_resources)
list(APPEND examples_resources ${resources})
file(COPY ${examples_resources} DESTINATION "resources/")
