
cmake_minimum_required(VERSION 3.12)

set(PROGRAM_NAME birb_project)

project(template_project
VERSION 1.0
LANGUAGES CXX)

# Headers
include_directories(birb2d/include/)

add_subdirectory(birb2d)

add_executable(${PROGRAM_NAME} src/main.cpp)
target_link_libraries(${PROGRAM_NAME} birb2d)
target_precompile_headers(${PROGRAM_NAME} PRIVATE birb2d/include/Birb2D.hpp)

# Copy the resources into the build directory
file(COPY ./res DESTINATION ./)
