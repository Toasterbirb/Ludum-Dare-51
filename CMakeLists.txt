cmake_minimum_required(VERSION 3.12)

set(PROGRAM_NAME diamond_heist)

project(template_project
VERSION 1.0
LANGUAGES CXX)

# Headers
include_directories(include/)
include_directories(birb2d/include/)


set(STATIC_SDL ON CACHE BOOL "Use static linking for SDL libraries")
set(TESTS OFF CACHE BOOL "Disable tests")
add_definitions(-DSTATIC_SDL)

add_subdirectory(birb2d)
include_directories(birb2d/vendor/SDL/include)


add_executable(${PROGRAM_NAME}
	src/main.cpp
	src/gamelevel.cpp
	src/resourceloader.cpp)
target_link_libraries(${PROGRAM_NAME} birb2d)
target_precompile_headers(${PROGRAM_NAME} PRIVATE birb2d/include/Birb2D.hpp)

# Copy the resources into the build directory
file(COPY ./res DESTINATION ./)
