cmake_minimum_required(VERSION 3.0.0)
project(EasyLinkSDK VERSION 1.0.0)

add_subdirectory("thirdparty/hidapi")
add_subdirectory("thirdparty/spdlog")

if(CMAKE_HOST_UNIX)
 
	link_libraries(hidapi_hidraw)
 
elseif(CMAKE_HOST_WIN32)
 
	link_libraries(hidapi)
 
else()
 
	link_libraries(hidapi)
 
endif()


# Debug mode
# ADD_DEFINITIONS(-D_DEBUG_FLAG)

link_libraries(spdlog_header_only)

set(SOURCE_FILES EasyLink.h EasyLink.cpp easy_link_c.cpp easy_link_c.h)

add_library(easylink SHARED ${SOURCE_FILES})
add_library(easylink_static STATIC ${SOURCE_FILES})
