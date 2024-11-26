cmake_minimum_required(VERSION 3.30)

project(tempest)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(tempest_engine)
add_subdirectory(tempest_app)
