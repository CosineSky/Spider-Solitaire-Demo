cmake_minimum_required(VERSION 3.24)
project(project C)

set(CMAKE_C_STANDARD 99)

add_executable(project main.c ui.h spider.h)

LINK_DIRECTORIES("./SDL2.dll")
target_link_libraries(project mingw32)
target_link_libraries(project SDL2main)
target_link_libraries(project SDL2)
target_link_libraries(project SDL2_image)
target_link_libraries(project SDL2_ttf)

