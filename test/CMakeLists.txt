# Add the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

find_package(pntr)
find_package(pntr_assetsys)

# Resources
file(GLOB resources resources/*)
set(test_resources)
list(APPEND test_resources ${resources})
file(COPY ${test_resources} DESTINATION "resources/")

# pntr_tiled_test
add_executable(pntr_tiled_test pntr_tiled_test.c)
target_link_libraries(pntr_tiled_test PUBLIC
    pntr
    pntr_tiled
    pntr_assetsys
)
set_property(TARGET pntr_tiled_test PROPERTY C_STANDARD 11)

# Set up the test
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
add_test(NAME pntr_tiled_test COMMAND pntr_tiled_test)
