cmake_minimum_required(VERSION 3.0.0)
project(EasyLinkSDK VERSION 1.0.0)

add_subdirectory("thirdparty/hidapi")
add_subdirectory("thirdparty/spdlog")

if(CMAKE_HOST_APPLE)
  link_libraries(hidapi)
elseif(CMAKE_HOST_UNIX)
  link_libraries(hidapi_hidraw)
elseif(CMAKE_HOST_WIN32)
  link_libraries(hidapi)
else()
  link_libraries(hidapi)
endif()


# Debug mode
# ADD_DEFINITIONS(-D_DEBUG_FLAG)

link_libraries(spdlog_header_only)

# Location of official SDK by Chessnut
add_subdirectory(sdk)

# Location of our own source files
add_subdirectory(src)
