cmake_minimum_required(VERSION 3.22)

project(malek_taj)

set(CMAKE_CXX_STANDARD 17)

add_executable(malek_taj main.cpp
        settings.cpp
        settings.f
        utils.cpp
        utils.f
        timed_set.cpp
        timed_set.f
)

target_link_libraries(malek_taj PRIVATE zmq)
