cmake_minimum_required(VERSION 3.20.0)

# `-DCMAKE_EXPORT_COMPILE_COMMANDS` generates `build/compile_commands.json`,
# which is needed for several tools including clang-tidy and the clangd LSP
# server.
# https://clangd.llvm.org/installation#compile_commandsjson
# https://clang.llvm.org/docs/JSONCompilationDatabase.html
# https://github.com/neovim/nvim-lspconfig/blob/master/doc/server_configurations.md#clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# https://cmake.org/cmake/help/latest/variable/CMAKE_CONFIGURATION_TYPES.html
# https://cmake.org/cmake/help/latest/generator/Ninja%20Multi-Config.html
# https://ninja-build.org/manual.html
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

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
