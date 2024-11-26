find_package(Vulkan REQUIRED)

add_executable(tempest_app
    src/main.cpp)

target_link_libraries(tempest_app PUBLIC tempest_engine)