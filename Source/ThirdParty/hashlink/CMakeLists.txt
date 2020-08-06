if (NOT URHO3D_HAXE_HASHLINK_HDLL)
set (TARGET_NAME hashlink)
cmake_minimum_required(VERSION 3.1)


set(HL_VERSION_MAJOR 1)
set(HL_VERSION_MINOR 12)
set(HL_VERSION_PATCH 0)
set(HL_VERSION ${HL_VERSION_MAJOR}.${HL_VERSION_MINOR}.${HL_VERSION_PATCH})

cmake_policy(SET CMP0042 NEW)


# force Unicode over Multi-byte
if(MSVC)
    add_definitions(-DUNICODE -D_UNICODE)
endif()


if(CMAKE_VERSION VERSION_LESS "3.1")
    set(CMAKE_C_FLAGS "-std=c11 ${CMAKE_C_FLAGS}")
else()
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif()


file(GLOB pcre_srcs
    include/pcre/*.c
)

file(GLOB std_srcs
    src/std/array.c
    src/std/buffer.c
    src/std/bytes.c
    src/std/cast.c
    src/std/date.c
    src/std/error.c
    src/std/file.c
    src/std/fun.c
    src/std/maps.c
    src/std/math.c
    src/std/obj.c
    src/std/random.c
    src/std/regexp.c
    src/std/socket.c
    src/std/string.c
    src/std/sys.c
    src/std/track.c
    src/std/types.c
    src/std/ucs2.c
    src/std/thread.c
    src/std/process.c
)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
if(NOT IOS)
    list(APPEND std_srcs
        include/mdbg/mdbg.c
        include/mdbg/mach_excServer.c
        include/mdbg/mach_excUser.c
    )
endif()   
endif()

if(ANDROID)
    list(APPEND std_srcs
        src/std/sys_android.c
    )
endif()

if(IOS)
    add_definitions(-DHL_MOBILE  -DHL_IOS)
    list(APPEND std_srcs
        src/std/sys_ios.m
    )
endif()



set (SOURCE_FILES ${pcre_srcs} src/gc.c ${std_srcs} src/hl.h src/hlc.h)

set (INCLUDE_DIRS src include include/pcre )


if(WIN32)
    list(APPEND  LIBS ws2_32 user32)
else()
    list(APPEND  LIBS m dl pthread)
endif()


#add_subdirectory(libs)


# Setup target
setup_library ()

#if (NOT URHO3D_HAXE_HASHLINK_HDLL)
endif() 

install_header_files (DIRECTORY src/ DESTINATION ${DEST_INCLUDE_DIR}/ThirdParty/hashlink FILES_MATCHING PATTERN *.h)  
