cmake_minimum_required(VERSION 3.15)
project(libmbsega)

# Require C++17 (and position independent code)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_SDL2 "Enables the SDL2 frontend (requires SDL2)." ON)

set(LIBMBSEGA_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/modules")
set(LIBMBSEGA_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/libmbsega/include" ${LIBMBSEGA_MODULES_DIR})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libmbsega/include/libmbsega" ${LIBMBSEGA_MODULES_DIR})

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(LIBMBSEGA_API_HEADERS
	libmbsega/include/libmbsega/libmbsega_api.h
	libmbsega/include/libmbsega/utils.h
	libmbsega/include/libmbsega/sms/mmu.h
	libmbsega/include/libmbsega/sms/vdp.h
	libmbsega/include/libmbsega/sms/psg.h
	libmbsega/include/libmbsega/sms/cpu.h
	libmbsega/include/libmbsega/libmbsega.h)

set(LIBMBSEGA
	libmbsega/src/core/sms/mmu.cpp
	libmbsega/src/core/sms/vdp.cpp
	libmbsega/src/core/sms/psg.cpp
	libmbsega/src/core/sms/cpu.cpp
	libmbsega/src/core/libmbsega.cpp)

if (BUILD_SDL2 STREQUAL "ON")
	message(STATUS "Building mbsega-SDL2...")
	add_subdirectory(mbsega-SDL2)
endif()

add_subdirectory(modules)

add_library(mbsega ${LIBMBSEGA} ${LIBMBSEGA_API_HEADERS})
target_include_directories(mbsega PUBLIC ${LIBMBSEGA_INCLUDE_DIRS})
target_compile_definitions(mbsega PRIVATE LIBMBSEGA_STATIC=1 _CRT_SECURE_NO_WARNINGS=1)
target_link_libraries(mbsega PRIVATE libbeez80 sn76489)
add_library(libmbsega ALIAS mbsega)

if (WIN32)
    message(STATUS "Operating system is Windows.")
    if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
	target_compile_options(mbsega PRIVATE -Wall)
	message(STATUS "Compiler is MinGW.")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
	message(WARNING "MSVC support is HIGHLY experimental, and may not even compile correctly, so be EXTREMELY careful here!")
	target_compile_options(mbsega PRIVATE /W4)
	message(STATUS "Compiler is MSVC.")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL Clang)
	message(WARNING "Clang support on Windows is HIGHLY experimental, and may not even compile correctly, so be EXTREMELY careful here!")
	target_compile_options(mbsega PRIVATE -Wall)
	message(STATUS "Compiler is Clang.")
    else()
	message(SEND_ERROR "Compiler not supported.")
	return()
    endif()
elseif(UNIX AND NOT APPLE)
    message(STATUS "Operating system is Linux.")
    if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
	target_compile_options(mbsega PRIVATE -Wall)
	message(STATUS "Compiler is GCC.")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL Clang)
	target_compile_options(mbsega PRIVATE -Wall)
	message(STATUS "Compiler is Clang.")
    else()
	message(SEND_ERROR "Compiler not supported.")
	return()
    endif()
elseif(APPLE)
    message(STATUS "Operating system is Mac.")
    if (CMAKE_CXX_COMPILER_ID STREQUAL AppleClang)
	target_compile_options(mbsega PRIVATE -Wall)
	message(STATUS "Compiler is AppleClang.")
    else()
	message(SEND_ERROR "Compiler not supported.")
	return()
    endif()
else()
    message(SEND_ERROR "Operating system not supported.")
    return()
endif()