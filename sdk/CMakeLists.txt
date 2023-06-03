# Official SDK by Chessnut
set(SDK_FILES EasyLink.h EasyLink.cpp easy_link_c.cpp easy_link_c.h)
add_library(libeasylink SHARED ${SDK_FILES})
add_library(libeasylink_static STATIC ${SDK_FILES})
