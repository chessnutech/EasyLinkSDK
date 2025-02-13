# Official SDK by Chessnut
#add_definitions(-D_DEBUG_FLAG=1)

set(SDK_FILES EasyLink.h EasyLink.cpp easy_link_c.cpp easy_link_c.h)

option(WITH_JAVA_BINDINGS "add Java glue code" OFF)

if(WITH_JAVA_BINDINGS)
# adjust path to jdk
        set(JDK_DIR "C:/Program Files (x86)/Java/jdk1.8.0_202")
        include_directories(
                "${JDK_DIR}/include"
                "${JDK_DIR}/include/win32")
        list(APPEND SDK_FILES EasyLink_java.cpp EasyLink_java.h)
endif()        

add_library(easylink SHARED ${SDK_FILES})
add_library(easylink_static STATIC ${SDK_FILES})
