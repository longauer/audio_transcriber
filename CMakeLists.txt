cmake_minimum_required(VERSION 3.23)
project(audio_transcriber)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_subdirectory(include)
add_subdirectory(src)