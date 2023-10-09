# Add the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

find_package(pntr)

# Resources
# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)

# pntr_tiled_test
add_executable(pntr_tiled_test pntr_tiled_test.c)
target_link_libraries(pntr_tiled_test PUBLIC
    pntr
    pntr_tiled
)
set_property(TARGET pntr_tiled_test PROPERTY C_STANDARD 99)

# Set up the test
add_test(NAME pntr_tiled_test COMMAND pntr_tiled_test)
