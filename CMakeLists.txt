cmake_minimum_required(VERSION 3.0.0)
project(EasyLinkSDK VERSION 1.0.0)

add_subdirectory("thirdparty/hidapi")
add_subdirectory("thirdparty/spdlog")

link_libraries(hidapi)

# Debug mode
# ADD_DEFINITIONS(-D_DEBUG_FLAG)

link_libraries(spdlog_header_only)

set(SOURCE_FILES EasyLink.h EasyLink.cpp easy_link_c.cpp easy_link_c.h)

add_library(libeasylink SHARED ${SOURCE_FILES})
add_library(libeasylink_static STATIC ${SOURCE_FILES})
