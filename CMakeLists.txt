cmake_minimum_required (VERSION 3.14)
project (xsvt)

set (CMAKE_C_STANDARD 99)
set (CMAKE_C_STANDARD_REQUIRED ON)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

option (DEBUG "Compile with debugging information." OFF)
option (BUILD_SHARED_LIBS "Compile shared lib, if OFF, static lib are compiled." OFF)

if (DEBUG)
    set (CMAKE_BUILD_TYPE "Debug")
else ()
    set(CMAKE_BUILD_TYPE "Release")
endif ()


if (UNIX)
    message ("Platform is UNIX")
    add_definitions (-D_GNU_SOURCE -D_REENTRANT)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64 -Wall -static-libgcc -fPIC")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -Wall -static-libgcc -static-libstdc++ -fPIC")
    if (DEBUG)
        add_definitions (-DDEBUG)
        set (CMAKE_C_FLAGS      "${CMAKE_C_FLAGS} -gdwarf-3 -O0")
        set (CMAKE_CXX_FLAGS    "${CMAKE_CXX_FLAGS} -gdwarf-3 -O0")
    else ()
        add_definitions (-DNDEBUG)
        set (CMAKE_C_FLAGS      "${CMAKE_C_FLAGS} -g0 -Ofast -minline-stringops-dynamically")
        set (CMAKE_CXX_FLAGS    "${CMAKE_CXX_FLAGS} -g0 -Ofast -minline-stringops-dynamically")
    endif ()
    add_compile_options("-fPIC")
    add_link_options("-Wl,-rpath=.")

elseif (WIN32)
    message ("Platform is Windows")
endif ()

message ("")
message ("CMAKE_C_STANDARD ${CMAKE_C_STANDARD}")
message ("CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD}\n")
message ("CMAKE_C_FLAGS ${CMAKE_C_FLAGS}")
message ("CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}\n")
message ("CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}\n")

include_directories ("../")

aux_source_directory (checker SRC_CHECKER)
message ("Found source ${SRC_CHECKER}")

aux_source_directory (onehot SRC_ONEHOT)
message ("Found source ${SRC_ONEHOT}")

aux_source_directory (transfer SRC_TRANSFER)
message ("Found source ${SRC_TRANSFER}")


if (BUILD_SHARED_LIBS)
    add_library (xsvt SHARED checker/checker.c onehot/onehot.c transfer/transfer.c)
else ()
    add_library (xsvt STATIC checker/checker.c onehot/onehot.c transfer/transfer.c)
endif ()

set (LIBXSVT xsvt)

if (DEBUG)
    add_executable (xsvt.checker.gcc.x64.d  ${SRC_CHECKER})
    add_executable (xsvt.onehot.gcc.x64.d  ${SRC_ONEHOT})
    add_executable (xsvt.transfer.gcc.x64.d  ${SRC_TRANSFER})
    target_link_libraries (xsvt.checker.gcc.x64.d  ${LIBXSVT})
    target_link_libraries (xsvt.onehot.gcc.x64.d  ${LIBXSVT})
    target_link_libraries (xsvt.transfer.gcc.x64.d  ${LIBXSVT})
else ()
    add_executable (xsvt.checker.gcc.x64  ${SRC_CHECKER})
    add_executable (xsvt.onehot.gcc.x64  ${SRC_ONEHOT})
    add_executable (xsvt.transfer.gcc.x64  ${SRC_TRANSFER})
    target_link_libraries (xsvt.checker.gcc.x64  ${LIBXSVT})
    target_link_libraries (xsvt.onehot.gcc.x64  ${LIBXSVT})
    target_link_libraries (xsvt.transfer.gcc.x64  ${LIBXSVT})
endif ()







