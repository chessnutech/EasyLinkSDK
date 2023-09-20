# Our own application
add_executable (main main.c)

# To find `easy_link_c.h` etc.; alternatively, "../sdk"
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/sdk")

target_link_libraries (main easylink)
