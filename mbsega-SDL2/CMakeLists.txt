project(mbsega-SDL2)

# Require C++17 (and position independent code)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(EXAMPLE_SOURCES 
	main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSDL_MAIN_HANDLED")

add_executable(${PROJECT_NAME} ${EXAMPLE_SOURCES})

include_directories(${LIBMBSEGA_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} libmbsega)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

if (TARGET SDL2::SDL2)
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2)
else()
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
endif()

